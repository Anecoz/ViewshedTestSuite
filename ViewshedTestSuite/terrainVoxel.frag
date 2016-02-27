// shadertype=glsl

#version 330

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 outColor;

uniform mat4 camMatrix;
uniform sampler3D voxTex;
uniform vec3 lightArr[10]; // World coordinates
uniform float maxDist; // Max distance for the viewshed

const vec3 lightDir = vec3(0.0, 30.0, 256.0);
const vec3 lightCol = vec3(1.0, 1.0, 1.0);

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
float rayMarch(vec3 lightPos) {	
	// Calculate if this particular terrain fragment is visible from the observer
	if (distance(fragPosition, lightPos) < maxDist) {
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
			float voxel = texture(voxTex, vec3(x,y,z)).r;	// Actual voxel at position, either 0 or 1

			if (voxel > 0.5) { // float precision...
				// We hit terrain, invisible
				return 0.0;
				break;
			}
			dist = distance(currPos, lightPos);
		}
	}
	// We are outside of maxdist, or made it through loop, set to visible
	return 1.0;
}

void main(void) {
	// Calculate lighting
	vec3 light = calcLight();

	// Loop over all lights
	float visibility = 0.0;
	for (int i = 0; i < 10; i++) {
		visibility += rayMarch(lightArr[i]);
	}

	// Color pixel depending on visibility
	visibility /= 10.0;

	if (distance(fragPosition, lightArr[4]) < maxDist) {
		outColor = vec4(vec3(visibility), 1.0);
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