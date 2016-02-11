// shadertype=glsl

#version 330

in vec3 fragPosition;
in vec3 fragNormal;

out vec4 outColor;

uniform mat4 camMatrix;

const vec3 lightDir = vec3(0.5, 0.5, 0.5);
const vec3 lightCol = vec3(1.0, 1.0, 1.0);

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

	total = specular*0.5 + diffuse + ambient;
	return total;
}

void main(void) {
	// Calculate lighting
	vec3 light = calcLight();	

	outColor = vec4(light*0.3, 1.0);
}