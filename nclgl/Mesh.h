#pragma once
#include "OGLRenderer.h"
#include <vector>


enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER, TEXTURE_BUFFER, NORMAL_BUFFER, TANGENT_BUFFER, INDEX_BUFFER, MODEL_BUFFER, MAX_BUFFER
};

class Mesh {
public:
	Mesh(GLuint type = GL_TRIANGLES, bool instanced = false, size_t numInstances = 0, std::vector<Matrix4> matrixes = {});
	~Mesh();

	virtual void Draw();
	
	static Mesh* GenerateQuad();
	static Mesh* GenerateQuad2();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateShape(std::vector<Vector3>& vertices, std::vector<Vector4>& colours, GLuint type);

	void SetColour(Vector4* cols) { colours = cols; };
	Vector4* GetColours() { return colours; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }

	void SetSecondaryTexture(GLuint tex) { secondTexture = tex; }
	GLuint GetSecondaryTexture() { return secondTexture; }

	void SetThirdTexture(GLuint tex) { thirdTexture = tex; }
	GLuint GetThirdTexture() { return thirdTexture; }
	
	void SetBumpMap(GLuint bump) { bumpTexture = bump; }
	GLuint GetBumpMap() const { return bumpTexture; }

	void SetMappedBuffer(bool mapped) { mappedBuffer = mapped; }

	void SetModelMatrix(std::vector<Matrix4>& model) { modelMatrixs = model; }


protected:

	void BufferData();
	void BufferDataMapped();

	void GenerateNormals();
	
	void GenerateTangents();
	Vector3 GenerateTangent(const Vector3& a, const Vector3& b, const Vector3& c, const Vector2& ta, const Vector2& tb, const Vector2& tc);

	bool mappedBuffer;
	bool instanced;

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint type;
	GLuint texture;
	GLuint secondTexture;
	GLuint bumpTexture;
	GLuint thirdTexture;

	Vector3* vertices;
	Vector4* colours;
	Vector2* textureCoords;
	Vector3* normals;
	Vector3* tangents;

	std::vector<Matrix4> modelMatrixs;
	size_t numInstances;


	size_t* indices;
	GLuint numIndices;
};