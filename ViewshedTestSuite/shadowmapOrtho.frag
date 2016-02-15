// shadertype=glsl

#version 330
out vec4 outColor;
in vec3 outPos;

void main() {
	gl_FragDepth = outPos.z; //is done automatically
	//outColor = vec4(vec3(outPos.z), 1.0);
	//outColor = vec4(vec3(gl_FragCoord.z), 1.0);
	//outColor = vec4(vec3(1.0), 1.0);
}