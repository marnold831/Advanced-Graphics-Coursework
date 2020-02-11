#include "Spline.h"
#include <math.h>

void Spline::Solution(){

	float t01 = powf(distance(points[0], points[1]), alpha);
	float t12 = powf(distance(points[0], points[1]), alpha);
	float t23 = powf(distance(points[0], points[1]), alpha);

	//Vector3 m1 = (1.0f - tension) * (points[2] - points[1] + t12 * ((points[1] - points[0]) / t01 - (points[2] - points[0]) / (t01 + t12)));

}

float Spline::distance(Vector3 a, Vector3 b)
{
	return powf((powf((b.x - a.x), 2) + powf((b.y - a.y), 2) + powf((b.z - a.z), 2)), 0.5);
}
