// shadertype=glsl

#version 330

in vec3 inPosition;
in vec3 inNormal;
out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 projMatrix;
uniform mat4 camMatrix;

void main(void) {
	fragPosition = inPosition;
	fragNormal = inNormal;
	gl_Position = projMatrix * camMatrix * vec4(inPosition, 1.0);
}