// shadertype=glsl

# version 430

flat in int f_axis;   //indicate which axis the projection uses
flat in vec4 f_AABB;

layout(pixel_center_integer) in vec4 gl_FragCoord;

in vec3 f_pos;

//layout(binding = 0, r8ui) writeonly restrict uniform uimage3D voxTex;
uniform layout(binding = 0, rgb10_a2ui) uimageBuffer voxelPos;
layout ( binding = 0, offset = 0 ) uniform atomic_uint voxelFragCount;

uniform int voxelDim;
uniform int shouldStoreVoxels;

void main()
{
    if( f_pos.x < f_AABB.x || f_pos.y < f_AABB.y || f_pos.x > f_AABB.z || f_pos.y > f_AABB.w )
	   discard ;

    //uvec4 data = uvec4(6);	// Something positive and != 0
	ivec4 temp = ivec4( gl_FragCoord.x, gl_FragCoord.y, voxelDim * gl_FragCoord.z, 0 ) ;
	uvec4 texcoord = uvec4(0);
	if( f_axis == 1 )
	{
	    texcoord.x = temp.z;
		texcoord.y = temp.y;
		texcoord.z = temp.x;
	}
	else if( f_axis == 2 )
    {
		texcoord.x = temp.x;
		texcoord.y = temp.z;
	    texcoord.z = temp.y;
	}
	else
	{
		//texcoord = temp;
		texcoord.x = voxelDim - temp.x;
		texcoord.y = temp.y;
		texcoord.z = temp.z;
	}	   
	 
	uint idx = atomicCounterIncrement(voxelFragCount);

	if (shouldStoreVoxels == 1) {
		//imageStore(voxTex, texcoord.xyz, data);
		imageStore(voxelPos, int(idx), texcoord);
	}	
}