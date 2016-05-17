// shadertype=glsl

#version 430

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in vec3 v_vertex[];

out vec3 fragPos;
out vec3 fragNormal;

void main() {
	fragNormal = normalize( cross(v_vertex[2]-v_vertex[0], v_vertex[1]-v_vertex[0] ) );

	fragPos = v_vertex[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();

	fragPos = v_vertex[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();

	fragPos = v_vertex[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}