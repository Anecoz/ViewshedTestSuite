// shadertype=glsl

#version 430

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

uniform int slice;

in vec3 v_vertex[];

out vec3 outPos;

void main() {
	// Check whether the triangle is too long (in theta-phi-space)
	// Phi is the one needed to be checked, corresponds to x-value of the vertex
	float ph1 = v_vertex[0].x;
	float ph2 = v_vertex[1].x;
	float ph3 = v_vertex[2].x;

	// Phi is scaled between -1,1 in vert stage
	float limit = 1.7;
	if (abs(ph1-ph2) > limit || abs(ph1-ph3) > limit || abs(ph2-ph3) > limit) {
		// Just don't output anything, effectively removing the primitive
		EndPrimitive();
	}
	else {

		// Set which layer of the depth attachment 2D arr texture to write to, and output vertices (basically pass-through)
		for (int i = 0; i < 3; i++) {
			gl_Layer = slice;
			gl_Position = gl_in[i].gl_Position;
			outPos = gl_in[i].gl_Position.xyz;
			EmitVertex();
		}

		EndPrimitive();
	}
}