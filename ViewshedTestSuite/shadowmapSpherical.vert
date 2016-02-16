// shadertype=glsl

#version 330

#define PI 3.1415926535897932384626433832795

in vec3 inPosition;
out vec3 outPos;

uniform vec3 lightPos; // World coordinates
uniform float maxDist; // max distance for the viewshed;
uniform mat4 modelMatrix; // unit for terrain, but not for camera quad

// arctan(y/x)
float atan2(in float y, in float x)
{
    return x == 0.0 ? sign(y)*PI/2 : atan(y, x);
}

// Performs conformal conic projection, i.e. maps spherical coords to a 2D surface
vec2 StereographicProjectionCon(vec3 sphericalCoords) {

	//float eps = 0.00000005;
	float oneSubZ = 1.0f - sphericalCoords.z;// + eps;
	return vec2(sphericalCoords.x/oneSubZ, sphericalCoords.y/oneSubZ) * 0.5f;
}

// Performs a very simple spherical projection (simply maps the two parameters to x and y)
vec2 StereographicProjectionSimple(vec3 sphericalCoords) {
	float x = sphericalCoords.x;
	float y = sphericalCoords.y;
	float z = sphericalCoords.z;

	// theta
	/*float theta;
	if (y > 0.0) {
		theta = atan(y,abs(x)) + PI/2.0;
	}
	else if (y < 0.0) {
		theta = PI/2.0 - atan(abs(y), abs(x));
	}
	else {
		theta = 0.0;
	}

	// Phi
	float phi;
	if (x > 0.0 && z > 0.0) {
		phi = atan(z,x);
	}
	else if (x < 0.0 && z > 0.0) {
		phi = PI - atan(z, abs(x));
	}
	else if (x < 0.0 && z < 0.0) {
		phi = PI + atan(abs(z), abs(x));
	}
	else if (x > 0.0 && z < 0.0) {
		phi = 2.0*PI - atan(abs(z), x);
	}
	else if (x == 0.0 && z == 0.0) {
		phi = 0.0;
	}
	else if (x == 0.0) {
		phi = PI/2.0;
	}
	else {
		phi = 0.0;
	}

	// Should be mapped between -1,1 in both directions
	theta = theta / PI;
	theta = 2.0*theta - 1.0;
	phi = phi / PI;
	phi = phi - 1.0;*/
	float theta = acos(y);
	float phi = atan(x, z);

	// Scale to -1,1
	theta = theta/PI;
	theta = 2.0*theta - 1.0;
	phi = phi/PI;

	return vec2(phi, theta);
}

void main() {
	vec3 vertPos = vec3( modelMatrix * vec4(inPosition, 1.0));
	vec3 sphericalCoords = normalize(vertPos - lightPos);
	outPos = vec3(StereographicProjectionSimple(sphericalCoords), distance(vertPos, lightPos)/maxDist);

	gl_Position = vec4(outPos, 1.0);
}