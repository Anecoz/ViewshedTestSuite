// shadertype=glsl

#version 430

in vec3 inPosition;
out vec3 v_vertex;

void main() {
	v_vertex = inPosition;
	gl_Position = vec4(inPosition, 1.0);
}