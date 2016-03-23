// shadertype=glsl

#version 430

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in vec3 v_vertex[];

out vec3 outPos;

void main() {
	// Check whether the triangle is too long (in theta-phi-space)
	// Phi is the one needed to be checked, corresponds to x-value of the vertex
	float ph1 = v_vertex[0].x;
	float ph2 = v_vertex[1].x;
	float ph3 = v_vertex[2].x;

	// Phi is scaled between -1,1 in vert stage
	float limit = 1.95;
	if (abs(ph1-ph2) > limit || abs(ph1-ph3) > limit || abs(ph2-ph3) > limit) {
		// Translate the primitive far off
		gl_Position = vec4(20*v_vertex[0], 1.0);
		outPos = 20*v_vertex[0];
		EmitVertex();

		gl_Position = vec4(20*v_vertex[1], 1.0);
		outPos = 20*v_vertex[1];
		EmitVertex();

		gl_Position = vec4(20*v_vertex[2], 1.0);
		outPos = 20*v_vertex[2];
		EmitVertex();

		EndPrimitive();
	}
	else {
		gl_Position = gl_in[0].gl_Position;
		outPos = gl_in[0].gl_Position.xyz;
		EmitVertex();

		gl_Position = gl_in[1].gl_Position;
		outPos = gl_in[1].gl_Position.xyz;
		EmitVertex();

		gl_Position = gl_in[2].gl_Position;
		outPos = gl_in[2].gl_Position.xyz;
		EmitVertex();

		EndPrimitive();
	}
}