#version 150

uniform sampler2D diffuseTex;
uniform sampler2D mergeTex;
uniform sampler2DShadow shadowTex;

uniform vec3 cameraPos;
uniform vec4 lightColor;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex{

	vec2 texCoord;
    vec4 colour;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
    vec4 shadowProj;

} IN;

out vec4 gl_FragColor;

void main(void){

	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 incident = normalize(lightPos - IN.worldPos);

	mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
	vec3 normal = normalize(TBN * (texture(mergeTex, IN.texCoord).rgb * 2.0 - 1.0));

	float lambert = max(0.0, dot(incident, normal));
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	vec3 viewDir = normalize(cameraPos - IN.worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float rFactor = max(0.0, dot(halfDir, normal));
	float sFactor = pow(rFactor, 50.0);

	float shadow = 1.0;

	if(IN.shadowProj.w > 0.0){
		shadow = textureProj(shadowTex, IN.shadowProj);
	}
	lambert *= shadow;

	vec3 colour = (diffuse.rgb * lightColor.rgb);
	colour += (lightColor.rgb * sFactor) * 0.33;

	vec4 fragColour = vec4(colour * atten * lambert, diffuse.a);
	fragColour.rgb += (diffuse.rgb * lightColor.rgb) * 0.1;
	
	gl_FragColor = fragColour;
    //gl_FragColor = diffuse;
	//gl_FragColor = vec4(IN.tangent, 1.0);
	//gl_FragColor = vec4(IN.normal, 1.0);
	//gl_FragColor = vec4(abs(IN.binormal), 1.0);
//gl_FragColor = vec4(lambert,lambert,lambert, 1.0);
//gl_FragColor = vec4(texture(bumpTex, IN.texCoord).rgb, 1.0);
}