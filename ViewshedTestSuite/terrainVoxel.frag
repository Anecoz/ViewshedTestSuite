// shadertype=glsl

#version 330

in vec3 fragPosition;
in vec3 fragNormal;
//in vec3 visibility;

out vec4 outColor;

uniform mat4 camMatrix;
uniform vec3 lightArr[100]; // World coordinates
uniform float maxDist; // Max distance for the viewshed
uniform int numObs;
uniform usampler3D voxTex; // 3D-texture of the voxels
uniform int voxelDim;
uniform float targetHeight;

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

// returns 1 if we are visible and 0 if we are invisible
int rayMarch(vec3 lightPos) {	
	// Calculate if this particular terrain fragment is visible from the observer
	float increment = 2.0;
	vec3 currPos = fragPosition + vec3(0, targetHeight, 0);
	vec3 direction = normalize(lightPos - currPos);	
	float minDist = 3.0;

	// Do the ray-marching
	float dist = distance(currPos, lightPos);
	while (dist > minDist) {
						// Increment 1 step
		//float x = floor(currPos.x)/512.0;			// Get the normalized sampling coords
		//float y = floor(currPos.y)/128.0;			// Ditto
		//float z = floor(currPos.z)/512.0;			// Ditto
		float x = currPos.x/float(voxelDim);
		float y = currPos.y/float(voxelDim);
		float z = currPos.z/float(voxelDim);
		uint voxel = texture(voxTex, vec3(x,y,z)).r;	// Actual voxel at position, either 0 or 1

		if (voxel > 0u) { // float precision...
			// We hit terrain, invisible
			return 0;
			break;
		}
		currPos += direction*increment;	
		dist = distance(currPos, lightPos);
	}
	// We made it through loop, set to visible
	return 1;
}

// Helpers for amanatide
int signum(float val) {
	if (val < 0.0) {return -1;}
	else {return 1;}
}

float calcMaxT(float dir, int i_coord, float f_coord) {
	if (dir < 0.0) {
		return (float(i_coord) - f_coord)/dir;
	}
	else {
		return (float(voxelDim) + float(i_coord) - f_coord)/dir;
	}
}

float intbounds(float s, float ds) 
{ 
	return (ds > 0? ceil(s)-s: s-floor(s)) / abs(ds); 
}

// Smarter ray traversal algorithm
int amanatideTraverse(vec3 lightPos) {
	vec3 currPos = fragPosition + vec3(0, targetHeight, 0);

	// Init phase
	int X = int(floor(currPos.x));
	int Y = int(floor(currPos.y));
	int Z = int(floor(currPos.z));

	// Origin (floats)
	float Ox = currPos.x;
	float Oy = currPos.y;
	float Oz = currPos.z;

	vec3 dir = normalize(lightPos - currPos); 
	float dx = dir.x;
	float dy = dir.y;
	float dz = dir.z;

	int StepX = signum(dx);
	int StepY = signum(dy);
	int StepZ = signum(dz);

	//float tMaxX = calcMaxT(dx, X, Ox);
	//float tMaxY = calcMaxT(dy, Y, Oy);
	//float tMaxZ = calcMaxT(dz, Z, Oz);

	float tMaxX = intbounds(Ox, dx);
	float tMaxY = intbounds(Oy, dy);
	float tMaxZ = intbounds(Oz, dz);

	float tDeltaX = float(StepX)/dx;
	float tDeltaY = float(StepY)/dy;
	float tDeltaZ = float(StepZ)/dz;

	// Loop phase
	float dist = distance(currPos, lightPos);
	float minDist = 3.0;

	while (dist > minDist) {
		if(tMaxX < tMaxY) {
			if(tMaxX < tMaxZ) {
				X= X + StepX;
				tMaxX= tMaxX + tDeltaX;
			} 
			else {
				Z= Z + StepZ;
				tMaxZ= tMaxZ + tDeltaZ;
			}
		} 
		else {
			if(tMaxY < tMaxZ) {
				Y= Y + StepY;
				tMaxY= tMaxY + tDeltaY;
			} 
			else {
				Z= Z + StepZ;
				tMaxZ= tMaxZ + tDeltaZ;
			}
		}

		// Do what you gotta do
		uint voxel = texture(voxTex, vec3(
		float(X)/float(voxelDim),
		float(Y)/float(voxelDim),
		float(Z)/float(voxelDim))).r;

		if (voxel > 0u) {
			// We hit terrain, invisible
			return 0;
			break;	// Safety harness
		}
		dist = distance(vec3(X, Y, Z), lightPos);
	}

	// We made it through the loop, so there was nothing in the way, return 1
	return 1;
}

void main(void) {
	// Calculate lighting
	vec3 light = calcLight();

	if (numObs > 0) {
		if (distance(fragPosition, lightArr[numObs/2]) < maxDist) {
			//outColor = vec4(visibility, 1.0);
			int totalVis = 0;
			for (int i = 0; i < numObs; i++) {
				//totalVis += rayMarch(lightArr[i]);
				totalVis += amanatideTraverse(lightArr[i]);
			}

			float value = float(totalVis) / float(numObs);

			outColor = vec4(vec3(value), 1.0);

		}
		else {
			outColor = vec4(light*0.3, 1.0);
		}
		//uint voxel = texture(voxTex, vec3(fragPosition.x/float(voxelDim),fragPosition.y/float(voxelDim),fragPosition.z/float(voxelDim))).r;
		//outColor = vec4(voxel);
	}
	else {
		outColor = vec4(light*0.3, 1.0);
	}	
}