#pragma once

#include "..\\..\\nclgl\OGLRenderer.h"
#include "..\\..\\nclgl\Camera.h"
#include "..\\..\\nclgl\SceneNode.h"
#include "..\\..\\nclgl\Frustum.h"
#include "..\\..\\nclgl\HeightMap.h"
#include <algorithm>

class Renderer : public OGLRenderer {

public:

	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void UpdateScene(float msec);
	virtual void RenderScene();
	void MoveLightToCameraPos();
	void SetCameraMode(bool mode);
	void SetLightRotation(bool mode);
	void SetGrowthInvert(bool mode);
	void SetStopGrowth(bool mode);
	void ChangeLightHeight(bool direction);

protected:

	void BuildNodeList(SceneNode* from);
	void SortNodeList();
	void ClearNodeList();
	void DrawNodes(bool shadows);
	void DrawNode(SceneNode* n, bool shadows);
	void FillShadowFBO();
	void FillProcessFBO();
	void FillPostProcessFBO();
	void DrawProcessFBO();
	void SetUpHeightmapToDraw(GLuint currentProg, SceneNode* node, bool shadows);
	void SetUpCubeToDraw(GLuint currentProg, SceneNode* node, bool shadows);
	vector<Matrix4> GenerateMatrices();
	void SetTexturesToRepeat(SceneNode* node);
	void UpdateTimeForGrowth(float msec);

	void GenerateShadowTex();
	void GenerateShadowFBO();
	void GenerateProcessTex();
	void GenerateProcessFBO();
	void GeneratePostProcessTex();
	void GeneratePostProcessFBO();


	SceneNode* root;
	Camera* camera; 
	Light* sceneLight;
	Mesh* quad;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint bufferTex;
	
	GLuint bufferFBO;


	GLuint processTex;
	GLuint processDepthTex;
	GLuint processFBO;

	GLuint postProcessTex;
	GLuint postProcessDepthTex;
	GLuint postProcessFBO;

	Shader* postProcessingShader;
	Shader* presentShader;


	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	Vector3 worldCenter;
	bool rotate;
	bool lightRotate;
	bool GrowthInvert;
	bool stopGrowth;

	float time;
	float totalTime = 25;
	float timePassed;
};
#pragma once

#include "..\\..\\nclgl\OGLRenderer.h"
#include "..\\..\\nclgl\Camera.h"
#include "..\\..\\nclgl\SceneNode.h"
#include "..\\..\\nclgl\Frustum.h"
#include "..\\..\\nclgl\HeightMap.h"
#include <algorithm>

class TempRenderer : public OGLRenderer {

public:

	TempRenderer(Window& parent);
	virtual ~TempRenderer();

	virtual void UpdateScene(float msec);
	virtual void RenderScene();
	void MoveLightToCameraPos();
	void SetCameraMode(bool mode);
	void SetLightRotation(bool mode);
	void SetGrowthInvert(bool mode);
	void SetStopGrowth(bool mode);
	void ChangeLightHeight(bool direction);

protected:

	void BuildNodeList(SceneNode* from);
	void SortNodeList();
	void ClearNodeList();
	void DrawNodes(bool shadows);
	void DrawNode(SceneNode* n, bool shadows);
	void FillShadowFBO();
	void FillProcessFBO();
	void FillPostProcessFBO();
	void DrawProcessFBO();
	void SetUpHeightmapToDraw(GLuint currentProg, SceneNode* node, bool shadows);
	void SetUpCubeToDraw(GLuint currentProg, SceneNode* node, bool shadows);
	vector<Matrix4> GenerateMatrices();
	void SetTexturesToRepeat(SceneNode* node);
	void UpdateTimeForGrowth(float msec);

	void GenerateShadowTex();
	void GenerateShadowFBO();
	void GenerateProcessTex();
	void GenerateProcessFBO();
	void GeneratePostProcessTex();
	void GeneratePostProcessFBO();


	SceneNode* root;
	Camera* camera;
	Light* sceneLight;
	Mesh* quad;

	GLuint shadowTex;
	GLuint shadowFBO;

	GLuint bufferTex;

	GLuint bufferFBO;


	GLuint processTex;
	GLuint processDepthTex;
	GLuint processFBO;

	GLuint postProcessTex;
	GLuint postProcessDepthTex;
	GLuint postProcessFBO;

	Shader* postProcessingShader;
	Shader* presentShader;


	Frustum frameFrustum;

	vector<SceneNode*> transparentNodeList;
	vector<SceneNode*> nodeList;

	Vector3 worldCenter;
	bool rotate;
	bool lightRotate;
	bool GrowthInvert;
	bool stopGrowth;

	float time;
	float totalTime = 25;
	float timePassed;
};