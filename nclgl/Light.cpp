#include "Light.h"

Light::Light(Vector3 position, Vector4 color,float radius): position(position), color(color), radius(radius) {
}
Light::Light() : position(Vector3(0,0,0)), color(Vector4(1,1,1,1)), radius(0.0f) {

}

Light::~Light() {}
