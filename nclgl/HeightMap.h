#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "Mesh.h"

#define RAW_WIDTH 257
#define RAW_HEIGHT 257

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Y 2.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f

class HeightMap : public Mesh {

public:

	HeightMap(std::string name, bool instanced = false, size_t numInstances = 0, std::vector<Matrix4> matrixes = {});
	~HeightMap();

	float GetMaxHeight()const { return maxHeight; }

	Light* GetLight()const { return light; }
	void SetLight(Light* _light) { light = _light; }

	static HeightMap* GenerateHeightMap(bool instanced = false, size_t numInstances = 0, std::vector<Matrix4> matrixes = {});

	

protected:
	
	float maxHeight;
	Light* light;


};