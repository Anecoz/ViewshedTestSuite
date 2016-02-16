// shadertype=glsl

#version 330
#define PI 3.1415926535897932384626433832795

in vec3 fragPosition;
in vec3 fragNormal;
in vec4 fragPositionLightSpace;

out vec4 outColor;

uniform mat4 camMatrix;
uniform sampler2D depthMap;
uniform vec3 lightPos; // World coordinates
uniform float maxDist; // Max distance for the viewshed

const vec3 lightDir = vec3(0.0, 30.0, 256.0);
const vec3 lightCol = vec3(1.0, 1.0, 1.0);

// arctan(y/x)
float atan2(in float y, in float x)
{
    return x == 0.0 ? sign(y)*PI/2 : atan(y, x);
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

// Performs conformal conic projection, i.e. maps spherical coords to a 2D surface
vec2 StereographicProjectionCon(vec3 sphericalCoords) {

	//float eps = 0.00000005;
	float oneSubZ = 1.0f - sphericalCoords.z;// + eps;
	return vec2(sphericalCoords.x/oneSubZ, sphericalCoords.y/oneSubZ) * 0.5f;
}

float shadowCalculation(vec3 fragPosLightSpace) {
	vec2 projCoords = StereographicProjectionSimple(fragPosLightSpace);
	float closestDepth = texture(depthMap, projCoords*0.5 + 0.5).r; 
	float currentDepth = distance(fragPosition, lightPos)/maxDist;
	float bias = 0.005; // To get rid of shadow acne
	
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	return shadow;
}

vec3 calcLight() {
	vec3 total = vec3(0.0);

	float ambient = 0.7;
	vec3 lightColor = normalize(lightCol);
	vec3 L = normalize(lightDir);
	vec3 N = fragNormal;

	//Diffuse
	float diffInt = max(dot(L, N), 0.01);
	vec3 diffuse = clamp(diffInt*lightCol, 0.0, 1.0);

	//Specular
	vec3 R = reflect(-L, N);
	vec3 V = normalize(-fragPosition);
	float specInt = max(dot(R,V), 0.0);
	specInt = pow(specInt, 20);
	vec3 specular = clamp(specInt*lightCol, 0.0, 1.0);

	// Calculate the shadow, but only if we are within the maximum distance of it
	float shadow;
	if (distance(fragPosition, lightPos) < maxDist) {
		shadow = shadowCalculation(normalize(fragPositionLightSpace.xyz));
	}
	else {
		shadow = 0.0;
	}

	if (shadow == 1.0) {
		total = vec3(1.0, 0.0, 0.0);
	}
	else {
		total = (specular*0.5 + diffuse) + ambient;
	}
	
	return total;
}

void main(void) {
	// Calculate lighting
	vec3 light = calcLight();
	outColor = vec4(light*0.3, 1.0);
	//outColor = vec4(vec3(distance(fragPosition, lightPos)/maxDist), 1.0);
	//outColor = vec4( vec3(fragPosition.x/512.0), 1.0);

	//vec3 projCoords = fragPositionLightSpace.xyz / fragPositionLightSpace.w;
	//projCoords = projCoords*0.5 + 0.5;
	//float test = texture(depthMap, fragPosition.xz/512.0).r;	
	//outColor = vec4(vec3(test), 1.0);
}