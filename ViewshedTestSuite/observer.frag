// shadertype=glsl

#version 430

out vec4 outColor;
in vec3 fragPos;
in vec3 fragNormal;
uniform vec3 color = vec3(1.0, 0.0, 0.0);

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
	vec3 V = normalize(-fragPos);
	float specInt = max(dot(R,V), 0.0);
	specInt = pow(specInt, 20);
	vec3 specular = clamp(specInt*lightCol, 0.0, 1.0);

	total = 0.3*((specular*0.5 + diffuse) + ambient);
	return total;
}

void main(void) {
	vec3 light = calcLight();
	outColor = vec4(light, 1.0);
}