// shadertype=glsl

#version 330

layout(location = 0) out vec3 color;
in vec3 fragPosition;

void main(void) {
	// Normalize coords
	float x = fragPosition.x/512.0;
	float y = fragPosition.y/128.0;
	float z = fragPosition.z/512.0;
	color = vec3(x, y, z);
}