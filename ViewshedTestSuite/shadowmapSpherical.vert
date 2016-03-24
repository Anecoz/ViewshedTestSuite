// shadertype=glsl

#version 430

#define PI 3.1415926535897932384626433832795

in vec3 inPosition;
out vec3 v_vertex;

uniform vec3 lightPos; // World coordinates
uniform float maxDist; // max distance for the viewshed;
uniform mat4 modelMatrix; // identity for terrain, but not for camera quad

// arctan(y/x)
float atan2(in float y, in float x)
{
    return x == 0.0 ? sign(y)*PI/2 : atan(y, x);
}

// Performs conformal conic projection, i.e. maps spherical coords to a 2D surface
vec2 StereographicProjectionCon(vec3 sphericalCoords) {

	//float eps = 0.00000005;
	float oneSubY = 1.0f - sphericalCoords.y;// + eps;
	return vec2(sphericalCoords.x/oneSubY, sphericalCoords.z/oneSubY);
}

// Performs a very simple spherical projection (simply maps the two parameters to x and y)
vec2 StereographicProjectionSimple(vec3 sphericalCoords) {
	float x = sphericalCoords.x;
	float y = sphericalCoords.y;
	float z = sphericalCoords.z;

	float theta = acos(z);
	float phi = atan(y, x);

	// Scale to -1,1
	theta = theta/PI;
	theta = 2.0*theta - 1.0;
	phi = phi/PI;

	return vec2(phi, theta);
}

void main() {
	vec3 vertPos = vec3( modelMatrix * vec4(inPosition, 1.0));
	vec3 sphericalCoords = normalize(vertPos - lightPos);
	v_vertex = vec3(StereographicProjectionCon(sphericalCoords), distance(vertPos, lightPos)/maxDist);

	gl_Position = vec4(v_vertex, 1.0);
}