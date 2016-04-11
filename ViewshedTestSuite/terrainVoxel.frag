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
uniform vec3 topNodePos;
uniform int voxTexDim;
uniform int topNodeSize;

const vec3 lightDir = vec3(0.0, 30.0, 256.0);
const vec3 lightCol = vec3(1.0, 1.0, 1.0);

// Octree stuff
// Calculates which octant a world coordinate falls into, given an octree node position
int getOctant(vec3 inPos, vec3 pos) {
	if (inPos.x <= pos.x && inPos.y <= pos.y && inPos.z >= pos.z) {
		return 1;
	} 
	else if (inPos.x >= pos.x && inPos.y <= pos.y && inPos.z >= pos.z) {
		return 2;
	}
	else if (inPos.x <= pos.x && inPos.y <= pos.y && inPos.z <= pos.z) {
		return 3;
	}
	else if (inPos.x >= pos.x && inPos.y <= pos.y && inPos.z <= pos.z) {
		return 4;
	}
	else if (inPos.x <= pos.x && inPos.y >= pos.y && inPos.z >= pos.z) {
		return 5;
	}
	else if (inPos.x >= pos.x && inPos.y >= pos.y && inPos.z >= pos.z) {
		return 6;
	}
	else if (inPos.x <= pos.x && inPos.y >= pos.y && inPos.z <= pos.z) {
		return 7;
	}
	else {
		return 8;
	}
}

// Calculates a child nodes position given its parents position and octant
vec3 getChildPos(vec3 pos, int octant, int size) {
	// Some helpers	
	float minX = pos.x - size/2.0;
	float minY = pos.y - size/2.0;
	float minZ = pos.z - size/2.0;

	float xHelp = (pos.x - minX) / 2.0;
	float yHelp = (pos.y - minY) / 2.0;
	float zHelp = (pos.z - minZ) / 2.0;

	switch (octant) {
	case 1:
		return vec3(minX + xHelp, minY + yHelp, pos.z + zHelp);
	case 2:
		return vec3(pos.x + xHelp, minY + yHelp, pos.z + zHelp);
	case 3:
		return vec3(minX + xHelp, minY + yHelp, minZ + zHelp);
	case 4:
		return vec3(pos.x + xHelp, minY + yHelp, minZ + zHelp);
	case 5:
		return vec3(minX + xHelp, pos.y + yHelp, pos.z + zHelp);
	case 6:
		return vec3(pos.x + xHelp, pos.y + yHelp, pos.z + zHelp);
	case 7:
		return vec3(minX + xHelp, pos.y + yHelp, minZ + zHelp);
	case 8:
		return vec3(pos.x + xHelp, pos.y + yHelp, minZ + zHelp);
	default:
		break;
	}
}

// calculate if a given world position is within a voxel or not, given octree structure
int posInsideVoxel(vec3 worldPos) {
	bool foundLeaf = false;

	int size = topNodeSize;
	vec3 currNodeWorldPos = topNodePos;
	uvec2 currNodeIndex = uvec2(0, 0);
	while (!foundLeaf) {
		int octant = getOctant(worldPos, currNodeWorldPos); // Between 1-8 inclusive
		uvec4 data = texture(voxTex, vec3(vec2(currNodeIndex)/float(voxTexDim-1.0f), float(octant-1.0f)/7.0f ));
		uvec2 childIndex = data.rg;
		uint isLeaf = data.a;

		if (isLeaf > 0u) {
			if (isLeaf == 1u) {
				foundLeaf = true;
				return 0;
			}
			else if (isLeaf == 2u) {
				foundLeaf = true;
				return 1;
			}
		}
		else {
			// Continue down the tree
			currNodeWorldPos = getChildPos(currNodeWorldPos, octant, size);
			size = size/2;
			currNodeIndex = childIndex;
		}
	}
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

	return (specular*0.5 + diffuse) + ambient;
}

// returns 1 if we are visible and 0 if we are invisible
int rayMarch(vec3 lightPos) {	
	// Calculate if this particular terrain fragment is visible from the observer
	float increment = .5;
	vec3 currPos = fragPosition + vec3(0, targetHeight, 0);
	vec3 direction = normalize(lightPos - currPos);	
	float minDist = 1.0;

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
		//uint voxel = texture(voxTex, vec3(x,y,z)).r;	// Actual voxel at position, either 0 or 1
		int insideVoxel = posInsideVoxel(vec3(x, y, z));

		if (insideVoxel == 1) {
			return 0;
			break;
		}

		/*if (voxel > 0u) { // float precision...
			// We hit terrain, invisible
			return 0;
			break;
		}*/
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
		/*uint voxel = texture(voxTex, vec3(
		float(X)/float(voxelDim),
		float(Y)/float(voxelDim),
		float(Z)/float(voxelDim))).r;

		if (voxel > 0u) {
			// We hit terrain, invisible
			return 0;
			break;	// Safety harness
		}*/

		int insideVoxel = posInsideVoxel(vec3(float(X), float(Y), float(Z)));
		if (insideVoxel == 1) {
			return 0;
			break;
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
		/*if (distance(fragPosition, lightArr[numObs/2]) < maxDist) {
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
		}*/
		// Try to color each fragment according to closest voxel
		int posInside = posInsideVoxel(fragPosition.xyz);
		outColor = vec4(posInside);

		//uint voxel = texture(voxTex, vec3(54.0f/float(voxTexDim-1.0f), 61.0f/float(voxTexDim-1.0f), 1.0f/7.0f) ).r;
		//uint voxel = texture(voxTex, vec3(0, 0, 0.45) ).r;
		//outColor = vec4(float(voxel)/255.0f);
	}
	else {
		outColor = vec4(light*0.3, 1.0);
	}	
}