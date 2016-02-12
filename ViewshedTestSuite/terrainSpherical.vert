// shadertype=glsl

#version 330

in vec3 inPosition;
in vec3 inNormal;
out vec3 fragPosition;
out vec4 fragPositionLightSpace;
out vec3 fragNormal;

uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform vec3 lightPos; // World coordinates

void main(void) {
	fragPosition = inPosition;
	fragPositionLightSpace = vec4(fragPosition - lightPos, 1.0);
	fragNormal = inNormal;
	gl_Position = projMatrix * camMatrix * vec4(inPosition, 1.0);
}