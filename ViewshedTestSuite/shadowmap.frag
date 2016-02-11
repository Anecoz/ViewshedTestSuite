// shadertype=glsl

#version 330
out vec4 outColor;

void main() {
	gl_FragDepth = gl_FragCoord.z; //is done automatically
	outColor = vec4(vec3(gl_FragCoord.z), 1.0);
}