// shadertype=glsl

# version 430

flat in int f_axis;   //indicate which axis the projection uses
flat in vec4 f_AABB;

layout(pixel_center_integer) in vec4 gl_FragCoord;

in vec3 f_pos;
//out vec4 outColor;

layout(binding = 0, r8ui) writeonly restrict uniform uimage3D voxTex;

uniform int voxelDim;

void main()
{
    if( f_pos.x < f_AABB.x || f_pos.y < f_AABB.y || f_pos.x > f_AABB.z || f_pos.y > f_AABB.w )
	   discard ;

    uvec4 data = uvec4(6);	// Something positive and != 0
	ivec4 temp = ivec4( gl_FragCoord.x, gl_FragCoord.y, voxelDim * gl_FragCoord.z, 0 ) ;
	ivec4 texcoord;
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
	
	imageStore(voxTex, texcoord.xyz, data);
	//outColor = vec4(texcoord.x/512.0);
}