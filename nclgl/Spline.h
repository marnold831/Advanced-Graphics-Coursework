#pragma once
#include "Vector3.h"
#include <vector>

struct Segment {
	Vector3 a;
	Vector3 b;
	Vector3 c;
	Vector3 d;
};

class Spline {

	Spline();
	~Spline();

	void Solution();

public:

protected:
	float distance(Vector3 a, Vector3 b);
	std::vector<Vector3> points;
	float alpha = 0.5;
	float tension = 0.0;
};