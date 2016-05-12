// shadertype=glsl

#version 330

out vec4 outColor;
in vec3 fragPos;

uniform sampler2DArray depthMap;
uniform bool isColor;

void main() {

	if (!isColor) {
		float depth = texture(depthMap, vec3(fragPos.xy*0.5 + 0.5, 0.0)).r;
		outColor = vec4( vec3(depth), 1.0);
	}
	else {
		outColor = texture(depthMap, vec3(fragPos.xy*0.5 + 0.5, 1.0));
	}
}