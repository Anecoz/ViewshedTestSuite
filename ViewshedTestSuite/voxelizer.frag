// shadertype=glsl

# version 430

flat in int f_axis;   //indicate which axis the projection uses
flat in vec4 f_AABB;

in vec3 f_pos;
//out vec4 outColor;

layout(binding = 0, r8ui) writeonly restrict uniform uimage3D voxTex;

uniform int voxelWidth;
uniform int voxelDepth;
uniform int voxelHeight;

void main()
{
    /*if( f_pos.x < f_AABB.x || f_pos.y < f_AABB.y || f_pos.x > f_AABB.z || f_pos.y > f_AABB.w )
	   discard ;*/

    uvec4 data = uvec4(6);
	ivec4 temp;
	if (f_axis == 1 || f_axis == 3) {
		temp = ivec4( gl_FragCoord.x, gl_FragCoord.y, voxelDepth * gl_FragCoord.z, 0 ) ;
	}
	else {
		temp = ivec4( gl_FragCoord.x, gl_FragCoord.y, voxelHeight * gl_FragCoord.z, 0 ) ;
	}
	ivec4 texcoord;
	if( f_axis == 1 )
	{
	    texcoord.x = temp.z;
		texcoord.y = temp.y;
		texcoord.z = voxelWidth - (voxelDepth - temp.x);
	}
	else if( f_axis == 2 )
    {
		texcoord.x = voxelWidth - temp.x;
		texcoord.y = temp.z;
	    texcoord.z = voxelWidth - temp.y;
	}
	else
	{
		//texcoord = temp;
		texcoord.x = voxelWidth - temp.x;
		texcoord.y = temp.y;
		texcoord.z = temp.z;
	}
	    
	
	imageStore(voxTex, texcoord.xyz, data);
	//outColor = vec4(texcoord.x/512.0);
}