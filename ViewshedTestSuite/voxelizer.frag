// shadertype=glsl

# version 430

flat in int f_axis;   //indicate which axis the projection uses
flat in vec4 f_AABB;

in vec3 f_pos;
out vec4 outColor;

layout(r32f) uniform image3D voxTex;

uniform int voxelDim;
uniform int voxelDepth;

void main()
{
    /*if( f_pos.x < f_AABB.x || f_pos.y < f_AABB.y || f_pos.x > f_AABB.z || f_pos.y > f_AABB.w )
	   discard ;*/

    vec4 data = vec4(1.0);
	ivec4 temp = ivec4( gl_FragCoord.x, gl_FragCoord.y, voxelDepth * gl_FragCoord.z, 0 ) ;
	ivec4 texcoord;
	if( f_axis == 1 )
	{
	    texcoord.x = temp.z;
		texcoord.y = temp.y;
		texcoord.z = voxelDepth - temp.x;
	}
	else if( f_axis == 2 )
    {
		texcoord.x = temp.x;
		texcoord.y = temp.z;
	    texcoord.z = voxelDepth - temp.y;		
	}
	else
	    texcoord = temp;
	
	imageStore(voxTex, texcoord.xyz, data);
	outColor = vec4(1.0);
}