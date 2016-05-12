// shadertype=glsl

#version 330
#define PI 3.1415926535897932384626433832795

in vec3 fragPosition;
in vec3 fragNormal;
//in vec4 fragPositionLightSpace;
//flat in float depth;

out vec4 outColor;

uniform mat4 camMatrix;
uniform sampler2DArray depthMap;
uniform vec3[50] lightArr; // World coordinates
uniform int numObs;
uniform float maxDist; // Max distance for the viewshed
uniform float targetHeight; // The height of the observer above ground

const vec3 lightDir = vec3(0.0, 30.0, 256.0);
const vec3 lightCol = vec3(1.0, 1.0, 1.0);

// arctan(y/x)
float atan2(in float y, in float x)
{
    return x == 0.0 ? sign(y)*PI/2 : atan(y, x);
}

// For mapping from grayscale to a jet colorspace
float interpolate( float val, float y0, float x0, float y1, float x1 ) {
    return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

float base( float val ) {
    if ( val <= -0.75 ) return 0;
    else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
    else if ( val <= 0.25 ) return 1.0;
    else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
    else return 0.0;
}

float red( float gray ) {
    return base( gray - 0.5 );
}
float green( float gray ) {
    return base( gray );
}
float blue( float gray ) {
    return base( gray + 0.5 );
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

// Performs conformal conic projection, i.e. maps spherical coords to a 2D surface
vec2 StereographicProjectionSouth(vec3 sphericalCoords) {
	float onePlusY = 1.0f + sphericalCoords.y;// + eps;
	return vec2(sphericalCoords.x/onePlusY, sphericalCoords.z/onePlusY);
}
vec2 StereographicProjectionNorth(vec3 sphericalCoords) {
	float oneSubY = 1.0f - sphericalCoords.y;// + eps;
	return vec2(sphericalCoords.x/oneSubY, sphericalCoords.z/oneSubY);
}

float shadowCalculation(vec3 fragPosLightSpace, int ind, vec3 lightPos) {
	vec2 projCoords;
	int north;
	// Check if we're using the north or south origin point
	if (fragPosLightSpace.y >= 0) {
		north = 0;
		if (fragPosLightSpace.y == 1.0) {
			projCoords = vec2(0, 0);
		}
		else {
			projCoords = StereographicProjectionSouth(fragPosLightSpace);
		}		
	}
	else {
		north = 1;
		if (fragPosLightSpace.y == 0.0) {
			projCoords = vec2(0, 0);
		}
		else {
			projCoords = StereographicProjectionNorth(fragPosLightSpace);
		}		
	}
	//uint closestDepth = textureLod(depthMap, vec3(projCoords*0.5 + 0.5, slice), 0.0).r;
	//float f_closestDepth = float(closestDepth)/255.0;
	projCoords = projCoords*0.5 + 0.5;
	float f_closestDepth;
	if (north > 0.5) {
		f_closestDepth = textureLod(depthMap, vec3(projCoords.x, projCoords.y, ind*2), 0.0).r;
	}
	else {
		f_closestDepth = textureLod(depthMap, vec3(projCoords.x, projCoords.y, ind*2 + 1), 0.0).r;
	}
	float currentDepth = distance(fragPosition, lightPos)/maxDist;
	float bias = 0.005; // To get rid of shadow acne
	
	float shadow = currentDepth - bias > f_closestDepth  ? 1.0 : 0.0;
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
	float shadow = 0.0;

	for (int ind = 0; ind < numObs; ind++) {
		vec3 currLightPos = lightArr[ind];			
		if (distance(fragPosition, currLightPos) < maxDist) {
			vec3 fragPosLightSpace = fragPosition - currLightPos;
			shadow += shadowCalculation(normalize(fragPosLightSpace + vec3(0.0, targetHeight, 0.0)), ind, currLightPos);
		}
	}

	if (shadow > 0.0) {
		total = 1.0 - vec3(shadow/float(numObs));

		// map to -1,1
		/*total = total*2.0 -1.0;
		float r = red(total.x);
		float g = green(total.x);
		float b = blue(total.x);
		total = vec3(r, g, b);*/

		//total = vec3(1.0, 0.0, 0.0);
	}
	else {
		if (numObs > 0 && distance(fragPosition, lightArr[numObs/2]) < maxDist) {
			total = vec3(1.0, 1.0, 1.0);
		}
		else {
			total = 0.3*((specular*0.5 + diffuse) + ambient);
		}		
	}
	
	return total;
}

void main(void) {
	// Calculate lighting
	vec3 light = calcLight();
	outColor = vec4(light, 1.0);

	/*if (numObs > 0) {
		float depth = textureLod(depthMap, vec3(fragPosition.x/512.0, fragPosition.z/512.0, 0), 0.0).r;
		outColor = vec4(vec3(depth), 1.0);
	}
	else {
		vec3 light = calcLight();
		outColor = vec4(light, 1.0);
	}*/
}