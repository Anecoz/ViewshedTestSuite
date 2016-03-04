// shadertype=glsl

#version 330

in vec3 inPosition;
in vec3 inNormal;
out vec3 fragPosition;
out vec3 fragNormal;
out vec3 visibility;

uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform usampler3D voxTex; // 3D-texture of the voxels
uniform vec3 lightArr[5]; // Observer positions in world coordinates
uniform float maxDist; // Max distance for the viewshed

const int NUM_OBS = 5; // Number of observers


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

// returns 1 if we are visible and 0 if we are invisible
int rayMarch(vec3 lightPos) {	
	// Calculate if this particular terrain fragment is visible from the observer
	float increment = 2.0;
	vec3 direction = normalize(lightPos - inPosition);
	vec3 currPos = inPosition;
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
		uint voxel = textureLod(voxTex, vec3(x,y,z), 0.0).r;	// Actual voxel at position, either 0 or 1

		if (voxel > 0u) { // float precision...
			// We hit terrain, invisible
			return 0;
			break;
		}
		dist = distance(currPos, lightPos);
	}
	// We made it through loop, set to visible
	return 1;
}

void main(void) {
	
	gl_Position = projMatrix * camMatrix * vec4(inPosition, 1.0);
	fragPosition = inPosition;
	fragNormal = inNormal;

	// Do a clipping check
	if (any(lessThan(gl_Position.xyz, vec3(-gl_Position.w))) ||
    any(greaterThan(gl_Position.xyz, vec3(gl_Position.w))))
	{
		// vertex will be clipped
		visibility = ivec3(0, 0, 0);
	}
	else {
		if (distance(inPosition, lightArr[NUM_OBS/2]) < maxDist) {
			// Do viewshed calculations for all observers
			int totalVis = 0;
			for (int i = 0; i < NUM_OBS; i++) {
				totalVis += rayMarch(lightArr[i]);
			}

			float value = float(totalVis) / float(NUM_OBS);

			// map to -1,1
			value = value*2.0 -1.0;
			float r = red(value);
			float g = green(value);
			float b = blue(value);
			visibility = vec3(r, g, b);
		}
		else {
			visibility = vec3(0, 0, 0);
		}
	}

	/*float x = inPosition.x/512.0;
	float y = inPosition.y/128.0;
	float z = inPosition.z/512.0;
	visibility = textureLod(voxTex, vec3(x,y,z), 0.0).r;
	//ivec3 visibility = ivec3(voxel);*/	
}