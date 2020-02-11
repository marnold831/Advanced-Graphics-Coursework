#version 150

uniform sampler2D withAA;
uniform sampler2D noAA;



in Vertex {
	
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void){


	vec4 wAA = texture(withAA, IN.texCoord).rgba;
	vec4 nAA = texture(noAA, IN.texCoord). rgba;

	gl_FragColor = vec4(abs(wAA.r - nAA.r), abs(wAA.g - nAA.g), abs(wAA.b - nAA.b), 1.0) * 10;
	gl_FragColor = texture ( withAA , IN.texCoord );
	//gl_FragColor = mix(texture(withAA, IN.texCoord), IN.colour, mixer);
}