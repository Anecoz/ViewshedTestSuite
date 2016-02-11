// shadertype=glsl

#version 330

in vec3 fragPosition;
in vec3 fragNormal;
in vec4 fragPositionLightSpace;

out vec4 outColor;

uniform mat4 camMatrix;
uniform sampler2D depthMap;

const vec3 lightDir = vec3(0.0, 30.0, 256.0);
//const vec3 lightDir = vec3(0.5, 0.5, 0.5);
const vec3 lightCol = vec3(1.0, 1.0, 1.0);

float shadowCalculation(vec4 fragPosLightSpace) {
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords*0.5 + 0.5;

	float closestDepth = texture(depthMap, projCoords.xy).r; 
	float currentDepth = projCoords.z; 
	
	float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
	return shadow;
}

vec3 calcLight() {
	vec3 total = vec3(0.0);

	float ambient = 0.7;
	vec3 lightPos = mat3(camMatrix)*lightDir;
	vec3 lightColor = normalize(lightCol);
	vec3 L = normalize(lightPos);
	vec3 N = mat3(camMatrix)*fragNormal;

	//Diffuse
	float diffInt = max(dot(L, N), 0.01);
	vec3 diffuse = clamp(diffInt*lightCol, 0.0, 1.0);

	//Specular
	vec3 R = reflect(-L, N);
	vec3 V = normalize(-fragPosition);
	float specInt = max(dot(R,V), 0.0);
	specInt = pow(specInt, 20);
	vec3 specular = clamp(specInt*lightCol, 0.0, 1.0);

	// Calculate the shadow
	float shadow = shadowCalculation(fragPositionLightSpace);
	//float shadow = 0.0;

	if (shadow == 1.0) {
		total = vec3(1.0, 0.0, 0.0);
	}
	else {
		total = (specular*0.5 + diffuse) + ambient;
	}
	
	//total = specular*0.5 + diffuse + ambient;
	return total;
}

void main(void) {
	// Calculate lighting
	vec3 light = calcLight();	
	outColor = vec4(light*0.3, 1.0);

	//vec3 projCoords = fragPositionLightSpace.xyz / fragPositionLightSpace.w;
	//projCoords = projCoords*0.5 + 0.5;
	//float test = texture(depthMap, fragPosition.xz/512.0).r;	
	//outColor = vec4(vec3(test), 1.0);
}