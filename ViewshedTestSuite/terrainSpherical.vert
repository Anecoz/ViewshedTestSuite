// shadertype=glsl

#version 330

in vec3 inPosition;
in vec3 inNormal;
out vec3 fragPosition;
//out vec4 fragPositionLightSpace;
out vec3 fragNormal;
//flat out float depth;

uniform mat4 projMatrix;
uniform mat4 camMatrix;
uniform vec3 lightPos; // World coordinates
//uniform sampler2D depthMap; // Test

void main(void) {
	//depth = textureLod(depthMap, vec2(inPosition.x/512.0, inPosition.z/512.0), 0).r;
	fragPosition = inPosition;
	//fragPositionLightSpace = vec4(fragPosition - lightPos, 1.0);
	fragNormal = inNormal;
	gl_Position = projMatrix * camMatrix * vec4(inPosition, 1.0);
}