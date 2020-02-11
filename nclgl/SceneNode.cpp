#include "SceneNode.h"

SceneNode::SceneNode(Mesh* m, Vector4 colour, std::string tag, Shader* shader, Shader* shadowShader) 
	: parent(nullptr), mesh(m), colour(colour), modelScale(Vector3(1, 1, 1)), distanceFromCamera(0.0f), 
	  boundingRadius(1.0f), tag(tag), shader(shader), shadowShader(shadowShader), textures(0) {
}

SceneNode::~SceneNode() {
	for (unsigned int i = 0; i < children.size(); ++i) {
		delete children[i];
	}
}

bool SceneNode::CompareByCameraDistance(SceneNode* a, SceneNode* b) {

	return(a->distanceFromCamera < b->distanceFromCamera) ? true : false;
}

void SceneNode::AddChild(SceneNode* s){
	children.emplace_back(s);
	s->parent = this;
}

void SceneNode::Update(float msec){

	if (parent)
		worldTransform = parent->worldTransform * transform;
	else {
		worldTransform = transform;
	}

	for (auto it : children) {
		it->Update(msec);
	}
}

void SceneNode::Draw(const OGLRenderer& r){
	if (mesh)
		mesh->Draw();
}

SceneNode* SceneNode::GenerateSceneNode(Mesh* _mesh, Matrix4 _transform, Vector3 _modelScale, Vector4 _color, float _radius, std::string tag) {
	SceneNode* node = new SceneNode();
	node->mesh =_mesh;
	node->transform = _transform;
	node->modelScale = _modelScale;
	node->colour = _color;
	node->boundingRadius = _radius;
	node->tag = tag;
	
	return node;
}

void SceneNode::BindTextures(GLuint prog) {
	for (size_t i = 0; i < textures.size(); ++i) {
		if (i == 0) {
			glUniform1i(glGetUniformLocation(prog, "diffuseTex"), i);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
		if (i == 1) {
			glUniform1i(glGetUniformLocation(prog, "mergeTex"), i);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
		if (i == 2) {

			glUniform1i(glGetUniformLocation(prog, "destroyTex"), i);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
		if (i == 3) {
			glUniform1i(glGetUniformLocation(prog, "grassTex"), i);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
	}
}

bool SceneNode::AddTexture(GLuint tex) {
	textures.push_back(tex);
	return tex;
}

