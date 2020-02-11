#pragma once

#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Mesh.h"
#include "OBJMesh.h"
#include <vector>

class SceneNode {

public :
	
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1), std::string tag = " ", Shader* shader = new Shader(SHADERDIR"debugVertex.glsl", SHADERDIR"debugFragment.glsl"), Shader* shadowShader = nullptr);
	~SceneNode();

	void setTransform(const Matrix4& matrix) { transform = matrix; }
	const Matrix4& GetTransform() const { return transform; }
	Matrix4 GetWorldTransfrom() const { return worldTransform; }

	Vector4 GetColour() const { return colour; }
	void SetColour(Vector4 c) { colour = c; }

	Vector3 GetModelScale() const { return modelScale; }
	void SetModelScale(Vector3 s) { modelScale = s; }

	Mesh* GetMesh() const { return mesh; }
	void SetMesh(Mesh* m) { mesh = m; }
	
	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float r) { boundingRadius = r; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float d) { distanceFromCamera = d; }

	std::string GetTag() const { return tag; }
	void SetTag(std::string _tag) { tag = _tag; }

	Shader* GetShader() const { return shader; }
	void SetShader(Shader* _shader) { shader = _shader; }
	
	Shader* GetShadowShader() const { return shadowShader; }
	void SetShadowShader(Shader* _shadowShader) { shadowShader = _shadowShader; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b);
	void AddChild(SceneNode* s);
	bool AddTexture(GLuint tex);

	GLuint GetTexture(int index) { return textures[index]; }
	int GetTextureSize() { return textures.size(); }

	virtual void Update(float msec);
	virtual void Draw(const OGLRenderer& r);

	static SceneNode* GenerateSceneNode(Mesh* mesh, Matrix4 transform, Vector3 modelScale, Vector4 color, float radius, std::string tag = " ");

	void BindTextures(GLuint prog);

	std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }

protected:

	SceneNode* parent;
	Mesh* mesh;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	float distanceFromCamera;
	float boundingRadius;
	std::vector<SceneNode*> children;

	Shader* shader;
	Shader* shadowShader;
	vector<GLuint> textures;

	std::string tag;
};