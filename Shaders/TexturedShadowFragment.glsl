#version 330 

in Vertex {

	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
	vec3 tangent;
	vec3 binormal;
	vec4 shadowProj;
	flat int instancedID;
} IN;

out vec4 fragColor;

void main(void) {
    fragColor = vec4(1.0);
}