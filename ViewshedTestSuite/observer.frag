// shadertype=glsl

#version 330

out vec4 outColor;
uniform vec3 color = vec3(1.0, 0.0, 0.0);

void main(void) {
	outColor = vec4(color, 1.0);
}