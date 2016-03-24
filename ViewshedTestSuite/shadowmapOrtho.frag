// shadertype=glsl

#version 430
in vec3 outPos;

//uniform int slice;

//layout(early_fragment_tests) in;
//layout(binding = 0, r32f) writeonly restrict uniform image3D depthMapTex;

void main() {
	gl_FragDepth = outPos.z;

	// Write the depth of this fragment at correct slice in the texture
	//imageStore(depthMapTex, ivec3(gl_FragCoord.x, gl_FragCoord.y, slice), vec4(outPos.z, 0, 0, 0));
}