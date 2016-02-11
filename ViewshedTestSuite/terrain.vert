// shadertype=glsl

#version 330

in vec3 inPosition;
in vec3 inNormal;
out vec3 fragPosition;
out vec4 fragPositionLightSpace;
out vec3 fragNormal;

uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform mat4 lightSpaceMatrix;

void main(void) {
	fragPosition = inPosition;
	fragPositionLightSpace = lightSpaceMatrix * vec4(fragPosition, 1.0);
	fragNormal = inNormal;
	gl_Position = projMatrix * camMatrix * vec4(inPosition, 1.0);
}