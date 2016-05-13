// shadertype=glsl

#version 430

layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 6 ) out;

in vec3 v_vertex[];
flat in int north[];

uniform int slice;
uniform vec3 lightPos; // World coordinates
uniform float maxDist; // max distance for the viewshed;

out vec3 outPos;

// Performs conformal conic projection, i.e. maps spherical coords to a 2D surface
vec2 StereographicProjectionSouth(vec3 sphericalCoords) {
	float onePlusY = 1.0f + sphericalCoords.y;// + eps;
	return vec2(sphericalCoords.x/onePlusY, sphericalCoords.z/onePlusY);
}
vec2 StereographicProjectionNorth(vec3 sphericalCoords) {
	float oneSubY = 1.0f - sphericalCoords.y;// + eps;
	return vec2(sphericalCoords.x/oneSubY, sphericalCoords.z/oneSubY);
}

void main() {
	// Check whether the triangle is too long (in theta-phi-space)
	// Phi is the one needed to be checked, corresponds to x-value of the vertex
	/*float ph1 = v_vertex[0].x;
	float ph2 = v_vertex[1].x;
	float ph3 = v_vertex[2].x;

	// Phi is scaled between -1,1 in vert stage
	float limit = 1.7;
	if (abs(ph1-ph2) > limit || abs(ph1-ph3) > limit || abs(ph2-ph3) > limit) {
		// Just don't output anything, effectively removing the primitive
		EndPrimitive();
	}
	else {*/

		// Set which layer of the depth attachment 2D arr texture to write to, and output vertices
		int northCounter = 0;
		for (int i = 0; i < 3; i++) {
			// Start by checking if all vertices are on the same side
			if (north[i] > 0.5) {
				northCounter++;
			}
		}

		// All north
		if (northCounter == 3) {
			for (int i = 0; i < 3; i++) {
				vec3 sphericalCoords = normalize(v_vertex[i] - lightPos);
				gl_Layer = slice;
				vec3 tmp = vec3(StereographicProjectionNorth(sphericalCoords), distance(v_vertex[i], lightPos)/maxDist);
				gl_Position = vec4(tmp, 1.0);
				outPos = tmp;
				EmitVertex();
			}
		}
		// All south
		else if (northCounter == 0) {
			for (int i = 0; i < 3; i++) {
				vec3 sphericalCoords = normalize(v_vertex[i] - lightPos);
				gl_Layer = slice + 1;
				vec3 tmp = vec3(StereographicProjectionSouth(sphericalCoords), distance(v_vertex[i], lightPos)/maxDist);
				gl_Position = vec4(tmp, 1.0);
				outPos = tmp;
				EmitVertex();
			}
		}
		// Else do it on both
		else {
			for (int i = 0; i < 3; i++) {
				vec3 sphericalCoords = normalize(v_vertex[i] - lightPos);
				gl_Layer = slice;
				vec3 tmp = vec3(StereographicProjectionNorth(sphericalCoords), distance(v_vertex[i], lightPos)/maxDist);
				gl_Position = vec4(tmp, 1.0);
				outPos = tmp;
				EmitVertex();
			}
			for (int i = 0; i < 3; i++) {
				vec3 sphericalCoords = normalize(v_vertex[i] - lightPos);
				gl_Layer = slice + 1;
				vec3 tmp = vec3(StereographicProjectionSouth(sphericalCoords), distance(v_vertex[i], lightPos)/maxDist);
				gl_Position = vec4(tmp, 1.0);
				outPos = tmp;
				EmitVertex();
			}
		}

		EndPrimitive();
	//}
}