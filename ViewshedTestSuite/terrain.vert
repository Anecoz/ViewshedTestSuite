// shadertype=glsl

#version 330

in vec3 inPosition;
out vec3 fragPosition;

uniform mat4 projMatrix;
uniform mat4 camMatrix;

void main(void) {
	fragPosition = inPosition;
	gl_Position = projMatrix * camMatrix * vec4(inPosition, 1.0);
	//gl_Position = vec4(inPosition, 1.0);
}