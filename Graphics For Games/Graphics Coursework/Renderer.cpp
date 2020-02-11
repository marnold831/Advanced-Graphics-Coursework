/* Author: Michael Arnold
   Student ID: 160155057
   Last Edited:  26 / 11/ 2019
   */

#include "Renderer.h"

#define SHADOWSIZE 2048

Renderer::Renderer(Window & parent) : OGLRenderer(parent), time(0.0), timePassed(0.0), GrowthInvert(false), stopGrowth(false) {

	root = new SceneNode();
	quad = Mesh::GenerateQuad2();

	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	camera = new Camera(-40, 270, Vector3(-2100, 3300, 2000), Vector3((RAW_HEIGHT * HEIGHTMAP_X) / 2, 500, (RAW_HEIGHT * HEIGHTMAP_X) / 2));

	vector<Matrix4> modelMatrices = GenerateMatrices();
	HeightMap* heightMap = new HeightMap(TEXTUREDIR"terrain.raw", true, modelMatrices.size(), modelMatrices);

	Light* light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 4000.0f, -(RAW_HEIGHT * HEIGHTMAP_Z) * 1.5), Vector4(1, 1, 1, 1), 100000);
	heightMap->SetLight(light);
	sceneLight = light;

	worldCenter = Vector3((RAW_HEIGHT * HEIGHTMAP_X) / 2.0f, 0.0, (RAW_HEIGHT * HEIGHTMAP_Z) / 2.0f);


	SceneNode* terrain = SceneNode::GenerateSceneNode(heightMap, Matrix4::Translation(Vector3(0, 0, 0)), Vector3(1, 1, 1), Vector4(1, 1, 1, 1), 100.0f, "heightmap");
	if (terrain->AddTexture(SOIL_load_OGL_texture(TEXTUREDIR"Stone.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)) == false) { return; }
	if (terrain->AddTexture(SOIL_load_OGL_texture(TEXTUREDIR"volcano.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)) == false) { return; }
	if (terrain->AddTexture(SOIL_load_OGL_texture(TEXTUREDIR"volcanoDead.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)) == false) { return; }
	if (terrain->AddTexture(SOIL_load_OGL_texture(TEXTUREDIR"Grass.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)) == false) {
		return;
	}

	SetTexturesToRepeat(terrain);

	terrain->SetShader(new Shader(SHADERDIR"InstancedVertex.glsl", SHADERDIR"TexturedFragment2.0.glsl"));
	terrain->SetShadowShader(new Shader(SHADERDIR"InstancedVertex.glsl", SHADERDIR"TexturedShadowFragment.glsl"));

	if (!terrain->GetShader()->LinkProgram() || !terrain->GetShadowShader()->LinkProgram())
		return;

	OBJMesh* cubeMesh = new OBJMesh();
	if (!cubeMesh->LoadOBJMesh(MESHDIR"cube.obj")) { return; }
	Mesh* cube = cubeMesh;

	SceneNode* cubeNode = SceneNode::GenerateSceneNode(cube, Matrix4::Translation(Vector3(500, 1000, 500)), Vector3(5, 5, 5), Vector4(1, 1, 1, 1), 100.0f, "cube");
	if (cubeNode->AddTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)) == false) { return; }
	if (cubeNode->AddTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS)) == false) { return; }
	cubeNode->SetShader(new Shader(SHADERDIR"CubeVertex.glsl", SHADERDIR"CubeFragment.glsl"));
	cubeNode->SetShadowShader(new Shader(SHADERDIR"CubeShadowVertex.glsl", SHADERDIR"CubeShadowFragment.glsl"));

	if (!cubeNode->GetShader()->LinkProgram() || !cubeNode->GetShadowShader()->LinkProgram())
		return;

	terrain->AddChild(cubeNode);

	root->AddChild(terrain);

	postProcessingShader = new Shader(SHADERDIR"PostProcessVertex.glsl", SHADERDIR"PostProcessFragment.glsl");
	presentShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");

	if (!postProcessingShader->LinkProgram() || !presentShader->LinkProgram())
		return;

	GenerateShadowTex();
	GenerateShadowFBO();
	GenerateProcessTex();
	GenerateProcessFBO();
	GeneratePostProcessTex();
	GeneratePostProcessFBO();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	init = true;
}
Renderer::~Renderer() {
	delete root;
	delete camera;
	delete sceneLight;

	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

}

void Renderer::UpdateScene(float msec) {
	root->Update(msec);
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);

	UpdateTimeForGrowth(msec);

	if (lightRotate) {
		Vector3 focusPoint = worldCenter;
		sceneLight->SetPosition((Matrix4::Rotation(-0.02f, Vector3(0, 1, 0)) * (sceneLight->GetPosition() - focusPoint)) + focusPoint);
	}

}

void Renderer::RenderScene() {
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);

	BuildNodeList(root);
	SortNodeList();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	projMatrix = Matrix4::Perspective(1.0f, 100000.0f, (float)width / (float)height, 45.0f);
	FillShadowFBO();
	FillProcessFBO();
	FillPostProcessFBO();
	DrawProcessFBO();


	SwapBuffers();
	ClearNodeList();
}

void Renderer::BuildNodeList(SceneNode* from) {


	Vector3 dir = from->GetWorldTransfrom().GetPositionVector() - camera->GetPosition();
	from->SetCameraDistance(Vector3::Dot(dir, dir));
	GLint alpha;

	if (from->GetMesh() && from->GetTexture(0)) {
		glBindTexture(GL_TEXTURE_2D, from->GetTexture(0));
		glGetTextureLevelParameterivEXT(from->GetMesh()->GetTexture(), GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_TYPE, &alpha);

		if (alpha != GL_NONE)
			transparentNodeList.push_back(from);

		else if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}


	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeList((*i));
	}
}

void Renderer::SortNodeList() {

	std::sort(transparentNodeList.begin(), transparentNodeList.end(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end()), SceneNode::CompareByCameraDistance;
}

void Renderer::ClearNodeList() {

	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawNodes(bool shadows) {

	for (vector<SceneNode*>::const_iterator i = nodeList.begin(); i != nodeList.end(); ++i) {
		DrawNode(*i, shadows);
	}

	for (vector<SceneNode*>::const_reverse_iterator i = transparentNodeList.rbegin(); i != transparentNodeList.rend(); ++i) {
		DrawNode(*i, shadows);
	}
}

void Renderer::DrawNode(SceneNode* node, bool shadows) {
	if (node->GetMesh()) {

		SetCurrentShader(shadows ? node->GetShadowShader() : node->GetShader());
		glUseProgram(currentShader->GetProgram());
		GLuint currentProg = currentShader->GetProgram();

		if (!shadows)
			viewMatrix = rotate ? Matrix4::BuildViewMatrix(camera->GetPosition(), Vector3((RAW_HEIGHT * HEIGHTMAP_X) / 2, 500, (RAW_HEIGHT * HEIGHTMAP_X) / 2)) : camera->BuildViewMatrix();

		UpdateShaderMatrices();

		if (node->GetTag() == "heightmap") {
			SetUpHeightmapToDraw(currentProg, node, shadows);

		}
		else if (node->GetTag() == "cube") {
			SetUpCubeToDraw(currentProg, node, shadows);
		}
		else {
			glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*) & (node->GetWorldTransfrom() * Matrix4::Scale(node->GetModelScale())));
			glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)& node->GetColour());
			glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)node->GetMesh()->GetTexture());
		}

		node->Draw(*this);
	}
	glUseProgram(0);
}

void Renderer::FillShadowFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	viewMatrix = Matrix4::BuildViewMatrix(sceneLight->GetPosition(), worldCenter);
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);

	DrawNodes(true);

	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::FillProcessFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawNodes(false);

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::FillPostProcessFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetCurrentShader(postProcessingShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	Vector2 screenDimension(width, height);
	glUniform2fv(glGetUniformLocation(currentShader->GetProgram(), "frameBufSize"), 1, (float*)& screenDimension);

	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, processTex);

	quad->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Renderer::DrawProcessFBO() {

	SetCurrentShader(presentShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "withAA"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "noAA"), 1);


	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcessTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, processTex);


	quad->Draw();

	glUseProgram(0);
	//glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
}

void Renderer::SetUpHeightmapToDraw(GLuint currentProg, SceneNode* node, bool shadows) {
	HeightMap* terrain = (HeightMap*)node->GetMesh();

	node->BindTextures(currentProg);

	modelMatrix.ToIdentity();
	Matrix4 tempMatrix = textureMatrix * modelMatrix;

	glUniform1f(glGetUniformLocation(currentProg, "maxHeight"), terrain->GetMaxHeight());
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	glUniformMatrix4fv(glGetUniformLocation(currentProg, "textureMatrix"), 1, false, *&tempMatrix.values);

	if (!shadows) {
		glUniform1i(glGetUniformLocation(currentProg, "shadowTex"), 4);
		glUniform3fv(glGetUniformLocation(currentProg, "cameraPos"), 1, (float*)& camera->GetPosition());

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowTex);
	}
	else {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	if (terrain->GetLight())
		SetShaderLight(*terrain->GetLight());
}

void Renderer::SetUpCubeToDraw(GLuint currentProg, SceneNode* node, bool shadows) {

	node->BindTextures(currentProg);

	Matrix4 transform = node->GetWorldTransfrom() * Matrix4::Scale(node->GetModelScale());
	Matrix4 tempMatrix = textureMatrix * transform;
	glUniformMatrix4fv(glGetUniformLocation(currentProg, "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentProg, "modelMatrix"), 1, false, (float*)& transform);

	if (!shadows) {
		glUniform1i(glGetUniformLocation(currentProg, "shadowTex"), 4);
		glUniform3fv(glGetUniformLocation(currentProg, "cameraPos"), 1, (float*)& camera->GetPosition());

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowTex);
	}
	else {
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	SetShaderLight(*sceneLight);
}

vector<Matrix4> Renderer::GenerateMatrices()
{
	Matrix4 modelA = Matrix4::Translation(Vector3(0, 0, 0));
	Matrix4 modelB = Matrix4::Translation(Vector3(-(RAW_HEIGHT * HEIGHTMAP_X - 17), 0, RAW_HEIGHT * HEIGHTMAP_X - 17));
	Matrix4 modelC = Matrix4::Translation(Vector3(0, 0, RAW_HEIGHT * HEIGHTMAP_Z - 17));
	Matrix4 modelD = Matrix4::Translation(Vector3(RAW_HEIGHT * HEIGHTMAP_X - 17, 0, RAW_HEIGHT * HEIGHTMAP_X - 17));
	Matrix4 modelE = Matrix4::Translation(Vector3(RAW_HEIGHT * HEIGHTMAP_X - 17, 0, 0));
	Matrix4 modelF = Matrix4::Translation(Vector3(RAW_HEIGHT * HEIGHTMAP_X - 17, 0, -(RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelG = Matrix4::Translation(Vector3(0, 0, -(RAW_HEIGHT * HEIGHTMAP_Z - 17)));
	Matrix4 modelH = Matrix4::Translation(Vector3(-(RAW_HEIGHT * HEIGHTMAP_X - 17), 0, -(RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelI = Matrix4::Translation(Vector3(-(RAW_HEIGHT * HEIGHTMAP_X - 17), 0, 0));
	Matrix4 modelJ = Matrix4::Translation(Vector3(-2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelK = Matrix4::Translation(Vector3(-2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, 2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelL = Matrix4::Translation(Vector3(-(RAW_HEIGHT * HEIGHTMAP_X - 17), 0, 2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelM = Matrix4::Translation(Vector3(0, 0, 2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelN = Matrix4::Translation(Vector3((RAW_HEIGHT * HEIGHTMAP_X - 17), 0, 2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelO = Matrix4::Translation(Vector3(2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, 2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelP = Matrix4::Translation(Vector3(2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelQ = Matrix4::Translation(Vector3(2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, 0));
	Matrix4 modelR = Matrix4::Translation(Vector3(2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, -(RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelS = Matrix4::Translation(Vector3(2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, -2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelT = Matrix4::Translation(Vector3(2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, -2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelU = Matrix4::Translation(Vector3((RAW_HEIGHT * HEIGHTMAP_X - 17), 0, -2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelV = Matrix4::Translation(Vector3(0, 0, -2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelW = Matrix4::Translation(Vector3(-(RAW_HEIGHT * HEIGHTMAP_X - 17), 0, -2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelX = Matrix4::Translation(Vector3(-2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, -2 * (RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelY = Matrix4::Translation(Vector3(-2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, -(RAW_HEIGHT * HEIGHTMAP_X - 17)));
	Matrix4 modelZ = Matrix4::Translation(Vector3(-2 * (RAW_HEIGHT * HEIGHTMAP_X - 17), 0, 0));

	vector<Matrix4> modelmatrix{ modelA, modelB, modelC, modelD, modelE, modelF, modelG, modelH, modelI, modelJ, modelK, modelL, modelM, modelN, modelO,
								 modelP, modelQ, modelR, modelS, modelT, modelU, modelV, modelW, modelX, modelY, modelZ };

	return modelmatrix;
}

void Renderer::SetTexturesToRepeat(SceneNode* node) {
	for (size_t i = 0; i < node->GetTextureSize(); ++i) {
		SetTextureRepeating(node->GetTexture(i), true);
	}
}

void Renderer::UpdateTimeForGrowth(float msec) {
	if (!stopGrowth) {
		if (GrowthInvert) {
			float previousTimePassed = timePassed;
			timePassed += msec;

			float differenceInTimePassed = timePassed - previousTimePassed;

			time -= (differenceInTimePassed * 0.001) / totalTime;
			if (time < 0)
				time = 0.0;
		}
		else
		{
			float previousTimePassed = timePassed;
			timePassed += msec;

			float differenceInTimePassed = timePassed - previousTimePassed;

			time += (differenceInTimePassed * 0.001) / totalTime;
			if (time > 1)
				time = 1.0;
		}
	}

}

void Renderer::GenerateShadowTex() {

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::GenerateShadowFBO() {

	glGenFramebuffers(1, &shadowFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);

	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GenerateProcessTex() {
	glGenTextures(1, &processTex);
	glBindTexture(GL_TEXTURE_2D, processTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &processDepthTex);
	glBindTexture(GL_TEXTURE_2D, processDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void Renderer::GenerateProcessFBO() {
	glGenFramebuffers(1, &processFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, processDepthTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !processDepthTex || !processTex)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GeneratePostProcessTex() {
	glGenTextures(1, &postProcessTex);
	glBindTexture(GL_TEXTURE_2D, postProcessTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenTextures(1, &postProcessDepthTex);
	glBindTexture(GL_TEXTURE_2D, postProcessDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void Renderer::GeneratePostProcessFBO() {
	glGenFramebuffers(1, &postProcessFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, postProcessDepthTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !postProcessDepthTex || !postProcessTex)
		return;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::MoveLightToCameraPos() {

	sceneLight->SetPosition(camera->GetPosition());

}

void Renderer::SetCameraMode(bool mode) {
	camera->SetRotate(mode);
	rotate = mode;
}

void Renderer::SetLightRotation(bool mode) {
	lightRotate = mode;
}

void Renderer::SetGrowthInvert(bool mode) {
	GrowthInvert = mode;
	timePassed = 0.0;
}

void Renderer::SetStopGrowth(bool mode) {
	stopGrowth = mode;
}

void Renderer::ChangeLightHeight(bool direction) {
	if (direction)
		sceneLight->SetPosition(sceneLight->GetPosition() + Vector3(0, 5, 0));
	else
		sceneLight->SetPosition(sceneLight->GetPosition() - Vector3(0, 5, 0));
}


