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
	flat int instancedID;
} OUT;

float GetHeight(vec2 coords) {
	
	vec4 textureA = texture(mergeTex, coords).rgba;
	vec4 textureB = texture(destroyTex, coords).rgba;
	vec4 mixture = mix(textureA, textureB, time);
	if(time > 0.67) {
		 return mixture.r * (3000 * time);
	}
	else {
		return mixture.r * 2000;
	}
	
}

void main(void) {
	
	mat4 newMatrix = textureMatrix * modelBuf;

	if(gl_InstanceID == 0) {

        vec2 coords = vec2(texCoord.x / 16.0f, texCoord.y / 16.0f);
       // vec4 textureA = texture(mergeTex, coords).rgba;
	    float height = GetHeight(coords);
        vec3 pos = vec3(position.x, height, position.z);

		vec2 neighbourXTexCoords = vec2((texCoord.x+(1/16.0f))/16.0f, texCoord.y / 16.0f);
		//vec4 neighbourXTexture = texture(mergeTex, neighbourXTexCoords).rgba;
		float heightX = GetHeight(neighbourXTexCoords);
		vec3 neighbourXPos = vec3(position.x + 16, heightX, position.z);

		vec2 neighbourZTexCoords = vec2((texCoord.x)/16.0f, (texCoord.y+(1/16.0f)) / 16.0f);
		//vec4 neighbourZTexture = texture(mergeTex, neighbourZTexCoords).rgba;
		float heightZ = GetHeight(neighbourZTexCoords);
		vec3 neighbourZPos = vec3(position.x, heightZ, position.z + 16.0f);

		vec3 tangent 	= normalize(neighbourXPos - pos);
		vec3 bitangent 	= normalize(neighbourZPos - pos);

		vec3 newNormal = normalize(cross(bitangent, tangent));
      	mat3 normalMatrix = transpose(inverse(mat3(modelBuf)));

	    OUT.normal =  normalize(normalMatrix * normalize(newNormal));
		OUT.tangent = normalize(normalMatrix * normalize(tangent));
		OUT.binormal = normalize(normalMatrix * normalize(bitangent));

		OUT.shadowProj = (newMatrix * vec4( pos +(newNormal * 10), 1));

	    mat4 mvp = projMatrix * viewMatrix * modelBuf;
	    gl_Position = mvp * vec4(pos, 1.0);

	    OUT.texCoord = texCoord;
	    OUT.worldPos = (modelBuf * vec4(pos, 1.0)).xyz;
	}

    else {
		vec2 coords = vec2(texCoord.x / 16.0f, texCoord.y / 16.0f);
       
        vec3 pos = vec3(position.x,0, position.z);
		vec3 neighbourXPos = vec3(position.x + 16, 0, position.z);
		vec3 neighbourZPos = vec3(position.x, 0, position.z + 16.0f);

		vec3 tangent 	= normalize(neighbourXPos - pos);
		vec3 bitangent 	= normalize(neighbourZPos - pos);

		vec3 newNormal = normalize(cross(bitangent, tangent));
      	mat3 normalMatrix = transpose(inverse(mat3(modelBuf)));

	    OUT.normal =  normalize(normalMatrix * normalize(newNormal));
		OUT.tangent = normalize(normalMatrix * normalize(tangent));
		OUT.binormal = normalize(normalMatrix * normalize(bitangent));

		OUT.shadowProj = (newMatrix * vec4( pos +(newNormal * 10), 1));

	    mat4 mvp = projMatrix * viewMatrix * modelBuf;
	    gl_Position = mvp * vec4(pos, 1.0);

	    OUT.texCoord = texCoord;
	    OUT.worldPos = (modelBuf * vec4(pos, 1.0)).xyz;
		OUT.instancedID = gl_InstanceID;
	}
 
}