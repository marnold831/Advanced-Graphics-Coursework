#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform float maxHeight;

uniform sampler2D mergeTex;
uniform sampler2D destroyTex;

uniform float time;

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;
layout (location = 6) in mat4 modelBuf;

out Vertex {

	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
	vec3 tangent;
	vec3 binormal;
	vec4 shadowProj;
} OUT;

float GetHeight(vec2 coords,  float time){

	vec4 textureA = texture(mergeTex, coords).rgba;
	vec4 textureB = texture(destroyTex, coords).rgba;
	float height;

	if(time >= 1.0){
		height = textureB.r * 2000;
	}
	else {
		vec4 mixture = mix(textureA, textureB, time);
		height = mixture.r * 2000;
	}
	return height;
}

void main(void) {

        vec2 coords = vec2(texCoord.x / 16.0f, texCoord.y / 16.0f);
        float newHeight = GetHeight(coords, time); 
        vec3 pos = vec3(position.x, newHeight, position.z);

		vec2 neighbourXTexCoords = vec2((texCoord.x+(1/16.0f))/16.0f, texCoord.y / 16.0f);
		float neighbourXHeight = GetHeight(neighbourXTexCoords, time);
		vec3 neighbourXPos = vec3(position.x + 16, neighbourXHeight, position.z);

		vec2 neighbourZTexCoords = vec2((texCoord.x)/16.0f, (texCoord.y+(1/16.0f)) / 16.0f);
		float neighbourZHeight = GetHeight(neighbourZTexCoords, time);
		vec3 neighbourZPos = vec3(position.x,neighbourZHeight, position.z + 16.0f);

		vec3 tangent 	= normalize(neighbourXPos - pos);
		vec3 bitangent 	= normalize(neighbourZPos - pos);

		vec3 newNormal = normalize(cross(bitangent, tangent));
      	mat3 normalMatrix = transpose(inverse(mat3(modelBuf)));

	    OUT.normal =  normalize(normalMatrix * normalize(newNormal));
		OUT.tangent = normalize(normalMatrix * normalize(tangent));
		OUT.binormal = normalize(normalMatrix * normalize(bitangent));


	    mat4 mvp = projMatrix * viewMatrix * modelBuf;
	    gl_Position = mvp * vec4(pos, 1.0);
	    OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	    OUT.worldPos = (modelBuf * vec4(pos, 1.0)).xyz;
 }
 