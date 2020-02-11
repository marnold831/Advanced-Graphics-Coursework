#version 330

uniform sampler2D diffuseTex;
uniform sampler2D grassTex;
uniform sampler2DShadow shadowTex;

uniform vec3 cameraPos;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform float lightRadius;

//uniform float mixer;

in Vertex {
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
	vec3 tangent;
	vec3 binormal;
	vec4 shadowProj;
	flat int instancedID;
} IN;

out vec4 gl_FragColor;

vec4 blend() {
	vec2 newTexCoords = vec2(IN.texCoord.x / 16, IN.texCoord.y /16);
	if(newTexCoords.x < 0.01) {
		if(newTexCoords.y <0.01) {
			vec4 cornerA = mix(texture(grassTex, newTexCoords), texture(grassTex, vec2(1, 1)), 0.5);
			vec4 cornerB = mix(texture(grassTex, vec2(0,1)), texture(grassTex, vec2(1, 0)), 0.5);
			return mix(cornerA, cornerB, 0.5);
			
		}
		else if(newTexCoords.y >0.99) {
			vec4 cornerA = mix(texture(grassTex,newTexCoords), texture(grassTex, vec2(1, 1)), 0.5);
			vec4 cornerB = mix(texture(grassTex, vec2(0,1)), texture(grassTex, vec2(1, 0)), 0.5);
			return mix(cornerA, cornerB, 0.5);
		}
		else {
			return mix(texture(grassTex, newTexCoords), texture(grassTex, vec2(1, newTexCoords.y)), 0.5);
			
		}
	}
	else if(newTexCoords.x > 0.99) {
		if(newTexCoords.y <0.01) {
			vec4 cornerA = mix(texture(grassTex, newTexCoords), texture(grassTex, vec2(0, 1)), 0.5);
			vec4 cornerB = mix(texture(grassTex, vec2(1,1)), texture(grassTex, vec2(0, 0)), 0.5);
			return mix(cornerA, cornerB, 0.5);
		}
		else if(newTexCoords.y >0.99) {
			vec4 cornerA = mix(texture(grassTex,newTexCoords), texture(grassTex, vec2(0, 0)), 0.5);
			vec4 cornerB = mix(texture(grassTex, vec2(0,1)), texture(grassTex, vec2(1, 0)), 0.5);
			return mix(cornerA, cornerB, 0.5);
		}
		else {
			return mix(texture(grassTex, newTexCoords), texture(grassTex, vec2(0, newTexCoords.y)), 0.5);
			
		}
	}

	else if(newTexCoords.y <0.01) {
		return mix(texture(grassTex, newTexCoords), texture(grassTex, vec2(newTexCoords.x, 1 - newTexCoords.y)), 0.5);
	}
	else if(newTexCoords.y >0.99) {
		return mix(texture(grassTex, newTexCoords), texture(grassTex, vec2(newTexCoords.x, 0 + (1-newTexCoords.y))), 0.5);
	}
	else {
		return texture(grassTex, newTexCoords);

	}
	

}

void main(void){
	vec4 diffuse;
	vec2 newTexCoords = vec2(IN.texCoord.x / 16, IN.texCoord.y /16);
	if(IN.instancedID > 0){
		diffuse = blend();
		//diffuse = vec4(IN.texCoord.x / 16, IN.texCoord.x / 16, IN.texCoord.x / 16, 1.0);
	}
	else
	{
		if(IN.worldPos.y == 0) {
			diffuse = texture(grassTex, newTexCoords);
		}
		else{
				diffuse = texture(diffuseTex, IN.texCoord);
		}
		
	}
	
	vec3 incident = normalize(lightPos - IN.worldPos);

	float lambert = 1;
	float atten = 1;
	float sFactor = 1;
	
	float dist = length(lightPos - IN.worldPos);
	lambert = max(0.0, dot(incident, IN.normal));
	atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float rFactor = max(0.0, dot(halfDir, IN.normal));
	sFactor = pow(rFactor, 50.0);

	float shadow =1.0;

	if(IN.shadowProj.w >= 0.0) {
		shadow = textureProj(shadowTex, IN.shadowProj);
		
	}
	

	lambert *= shadow;


	vec3 colour = (diffuse.rgb * lightColor.rgb);
	colour += (lightColor.rgb * sFactor) * 0.6;
	vec4 fragColour = vec4(colour * atten * lambert, diffuse.a);
	fragColour.rgb += (diffuse.rgb * lightColor.rgb) * 0.1;
	
	gl_FragColor = fragColour;
	//gl_FragColor = vec4(IN.normal, 1.0);
	//gl_FragColor = vec4(IN.tangent, 1.0);
	//gl_FragColor = vec4(IN.binormal, 1.0);
	//gl_FragColor = vec4(vec3(IN.instanceID) / 2, 1.0);
}	