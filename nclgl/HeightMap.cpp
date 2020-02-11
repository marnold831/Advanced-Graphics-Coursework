#include "HeightMap.h"

HeightMap::HeightMap(std::string name, bool instanced, size_t numInstances, std::vector<Matrix4> matrixes): Mesh(GL_TRIANGLES, instanced, numInstances, matrixes), maxHeight(0), light(nullptr){

	std::ifstream file(name.c_str(), std::ios::binary);
	if (!file) 
		return;

	numVertices = RAW_WIDTH * RAW_HEIGHT;
	numIndices = (RAW_WIDTH - 1) * (RAW_HEIGHT - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	unsigned char* data = new unsigned char[numVertices];
	file.read((char*)data, numVertices * sizeof(unsigned char));
	file.close();

	for (int x = 0; x < RAW_WIDTH; ++x) {
		for (int z = 0; z < RAW_HEIGHT; ++z) {

			int offset = (x * RAW_WIDTH) + z;

			float currentHeight = data[offset] * HEIGHTMAP_Y;

			if (maxHeight <currentHeight)
				maxHeight = currentHeight;

			vertices[offset] = Vector3(x * HEIGHTMAP_X, currentHeight, z * HEIGHTMAP_Z);
			textureCoords[offset] = Vector2(x * HEIGHTMAP_TEX_X, z * HEIGHTMAP_TEX_Z);
		}
	}
	delete data;

	numIndices = 0;

	for (int x = 0; x < RAW_WIDTH - 1; ++x) {
		for (int z = 0; z < RAW_HEIGHT - 1; ++z) {

			int a = (x * RAW_WIDTH) + z;
			int b = ((x + 1) * RAW_WIDTH) + z;
			int c = ((x + 1) * RAW_WIDTH) + (z + 1);
			int d = (x * RAW_WIDTH) + (z + 1);

			indices[numIndices++] = c;
			indices[numIndices++] = b;
			indices[numIndices++] = a;

			indices[numIndices++] = a;
			indices[numIndices++] = d;
			indices[numIndices++] = c;
		}
	}
	GenerateNormals();
	GenerateTangents();
	BufferData();
}

HeightMap::~HeightMap()
{
}

HeightMap* HeightMap::GenerateHeightMap(bool instanced , size_t numInstances, std::vector<Matrix4> matrixes)
{
	HeightMap* heightMap = new HeightMap(TEXTUREDIR"terrain.raw", instanced, numInstances, matrixes);
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetSecondaryTexture(SOIL_load_OGL_texture(TEXTUREDIR"volcano.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetThirdTexture(SOIL_load_OGL_texture(TEXTUREDIR"volcanoDead.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	return heightMap;
}
