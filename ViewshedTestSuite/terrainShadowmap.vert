// shadertype=glsl

#version 330

in vec3 inPosition;
out vec3 fragPos;

void main() {
	fragPos = inPosition;
	gl_Position = vec4(inPosition, 1.0);
}