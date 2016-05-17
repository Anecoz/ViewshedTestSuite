// shadertype=glsl

#version 430

in vec3 inPosition;

out vec3 v_vertex;

uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 modelMatrix;

uniform int scale = 1;

void main(void) {
	vec3 Vertex = inPosition;
	Vertex.x *= scale;
	Vertex.y *= scale;
	Vertex.z *= scale;
	vec3 fragPos = (modelMatrix*vec4(Vertex, 1.0)).xyz;
	gl_Position = projMatrix * camMatrix * modelMatrix * vec4(Vertex, 1.0);
	v_vertex = fragPos;
	//gl_Position = vec4(inPosition, 1.0);
}