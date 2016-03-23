// shadertype=glsl

#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in vec3 visibility;

out vec4 outColor;

uniform mat4 camMatrix;
uniform vec3 lightArr[100]; // World coordinates
uniform float maxDist; // Max distance for the viewshed
uniform int numObs;
uniform usampler3D voxTex; // 3D-texture of the voxels
uniform int voxelDim;

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

void main(void) {
	// Calculate lighting
	vec3 light = calcLight();

	if (numObs > 0) {
		/*if (distance(fragPosition, lightArr[numObs/2]) < maxDist) {
			outColor = vec4(visibility, 1.0);
		}
		else {
			outColor = vec4(light*0.3, 1.0);
		}*/
		uint voxel = textureLod(voxTex, vec3(fragPosition.x/float(voxelDim),fragPosition.y/float(voxelDim),fragPosition.z/float(voxelDim)), 0.0).r;
		outColor = vec4(voxel);
	}
	else {
		outColor = vec4(light*0.3, 1.0);
	}	
}