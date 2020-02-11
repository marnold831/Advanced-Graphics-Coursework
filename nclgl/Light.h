#pragma once

#include "Vector3.h"
#include "Vector4.h"

class Light {

public:
	Light(Vector3 position, Vector4 color, float radius);
	Light();
	~Light();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 _position) { position = _position; }

	float GetRadius() const { return radius; }
	void SetRadius(float _radius) { radius = _radius; }

	Vector4 GetColor() const { return color; }
	void SetColor(Vector4 _color) { color = _color; }

	

protected:

	Vector3 position;
	Vector4 color;
	
	float radius;
};