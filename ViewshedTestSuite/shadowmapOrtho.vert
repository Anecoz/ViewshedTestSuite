// shadertype=glsl

#version 330

in vec3 inPosition;

uniform mat4 lightSpaceMatrix;

void main() {
	gl_Position = lightSpaceMatrix * vec4(inPosition, 1.0f);
}