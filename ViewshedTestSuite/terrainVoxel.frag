// shadertype=glsl

#version 330

in vec3 fragPosition;
in vec3 fragNormal;
flat in vec3 visibility;

out vec4 outColor;

uniform mat4 camMatrix;
uniform usampler3D voxTex;
uniform vec3 lightArr[5]; // World coordinates
uniform float maxDist; // Max distance for the viewshed

const vec3 lightDir = vec3(0.0, 30.0, 256.0);
const vec3 lightCol = vec3(1.0, 1.0, 1.0);

const int NUM_OBS = 5;

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

	return (specular*0.5 + diffuse) + ambient;
}

// Returns 1 if we are visible and 0 if we are invisible
int rayMarch(vec3 lightPos) {	
	float increment = 1.5;
	vec3 direction = normalize(lightPos - fragPosition);
	vec3 currPos = fragPosition;
	float minDist = 3.0;

	// Do the ray-marching
	float dist = distance(currPos, lightPos);
	while (dist > minDist) {
		currPos += direction*increment;					// Increment 1 step
		//float x = floor(currPos.x)/512.0;			// Get the normalized sampling coords
		//float y = floor(currPos.y)/128.0;			// Ditto
		//float z = floor(currPos.z)/512.0;			// Ditto
		float x = currPos.x/512.0;
		float y = currPos.y/128.0;
		float z = currPos.z/512.0;
		uint voxel = texture(voxTex, vec3(x,y,z)).r;	// Actual voxel at position, either 0 or 1

		if (voxel > 0u) { // float precision...
			// We hit terrain, invisible
			return 0;
			break;
		}
		dist = distance(currPos, lightPos);
	}
	// We are outside of maxdist, or made it through loop, set to visible
	return 1;
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

void main(void) {
	// Calculate lighting
	vec3 light = calcLight();

	if (distance(fragPosition, lightArr[NUM_OBS/2]) < maxDist) {
		outColor = vec4(visibility, 1.0);
		/*float x = floor(fragPosition.x)/512.0;
		float y = floor(fragPosition.y)/128.0;
		float z = floor(fragPosition.z)/512.0;
		uint voxel = texture(voxTex, vec3(x,y,z)).r;
		outColor = vec4(vec3(voxel), 1.0);*/
		
		// Do viewshed calculations for all observers
		/*int totalVis = 0;
		for (int i = 0; i < NUM_OBS; i++) {
			totalVis += rayMarch(lightArr[i]);
		}

		float value = float(totalVis) / float(NUM_OBS);

		outColor = vec4(vec3(value), 1.0);*/
	}
	else {
		outColor = vec4(light*0.3, 1.0);
	}

	/*if (visibility < 0.5) {
		outColor = vec4(vec3(1.0, 0.0, 0.0)*0.3, 1.0);
	}
	else {
		outColor = vec4(light*0.3, 1.0);
	}*/
}