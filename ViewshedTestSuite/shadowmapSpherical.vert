// shadertype=glsl

#version 330

in vec3 inPosition;

uniform vec3 lightPos; // World coordinates

// Performs conformal conic projection, i.e. maps spherical coords to a 2D surface
vec2 StereographicProjection(vec3 sphericalCoords) {

	float eps = 0.00000005;
	float oneSubZ = 1.0f - sphericalCoords.z + eps;
	return vec2(sphericalCoords.x/oneSubZ, sphericalCoords.y/oneSubZ) * 0.5f;
}

void main() {
	vec3 sphericalCoords = normalize(inPosition - lightPos);
	vec3 outPos = vec3(StereographicProjection(sphericalCoords), distance(inPosition, lightPos)/128.0);

	gl_Position = vec4(outPos, 1.0);
}