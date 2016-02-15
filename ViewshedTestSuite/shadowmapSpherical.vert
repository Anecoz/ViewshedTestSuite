// shadertype=glsl

#version 330

in vec3 inPosition;

uniform vec3 lightPos; // World coordinates
uniform float maxDist; // max distance for the viewshed;
uniform mat4 modelMatrix; // unit for terrain, but not for camera quad

// Performs conformal conic projection, i.e. maps spherical coords to a 2D surface
vec2 StereographicProjection(vec3 sphericalCoords) {

	float eps = 0.00000005;
	float oneSubZ = 1.0f - sphericalCoords.z + eps;
	return vec2(sphericalCoords.x/oneSubZ, sphericalCoords.y/oneSubZ) * 0.5f;
}

void main() {
	vec3 vertPos = vec3( modelMatrix * vec4(inPosition, 1.0));
	vec3 sphericalCoords = normalize(vertPos - lightPos);
	vec3 outPos = vec3(StereographicProjection(sphericalCoords), distance(vertPos, lightPos)/maxDist);

	gl_Position = vec4(outPos, 1.0);
}