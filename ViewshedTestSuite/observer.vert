// shadertype=glsl

#version 330

in vec3 inPosition;

uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 modelMatrix;

void main(void) {
	gl_Position = projMatrix * camMatrix * modelMatrix * vec4(inPosition, 1.0);
	//gl_Position = vec4(inPosition, 1.0);
}