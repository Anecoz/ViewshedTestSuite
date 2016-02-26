#include "VoxelViewshed.h"


VoxelViewshed::VoxelViewshed()
{
	this->observer.setPos(glm::vec3(256, 20, 256));
}

VoxelViewshed::~VoxelViewshed()
{

}

void VoxelViewshed::init() {
	observer.init();
}

void VoxelViewshed::render(glm::mat4 projMatrix, Camera* camera) {
	observer.render(projMatrix, camera);
}

glm::vec3 VoxelViewshed::getPos() {
	return observer.getPos();
}

GLuint& VoxelViewshed::getVoxelTexture(VoxelContainer& voxels) {
	GLfloat *arr = voxels.getArray();

	const GLuint WIDTH = voxels.getDim();
	const GLuint HEIGHT = voxels.getHeight();
	const GLuint DEPTH = WIDTH;

	// Generate a 3D texture
	GLuint voxTex;
	
	glGenTextures(1, &voxTex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, WIDTH, HEIGHT, DEPTH, 0, GL_RED,
		GL_FLOAT, arr);

	// DEBUG
	/*GLubyte *testArr = (GLubyte *)malloc(sizeof(GLubyte) * WIDTH * DEPTH * HEIGHT);
	glGetTextureImage(voxTex, 0, GL_RED, GL_UNSIGNED_BYTE, WIDTH*HEIGHT*DEPTH*sizeof(GLubyte), testArr);
	printf("texture at 7, 11, 178 is: %d\n", testArr[HEIGHT*WIDTH * 178 + DEPTH * 11 + 7]);

	/*for (int x = 0; x < 512; x++)
		for (int y = 0; y < 128; y++)
			for (int z = 0; z < 512; z++) {
				printf("texture at %d, %d, %d is %d\n", x, y, z, testArr[HEIGHT*WIDTH*z + DEPTH*y + x]);
			}*/

	return voxTex;
}

void VoxelViewshed::tick(KeyboardHandler* handler) {
	observer.tick(handler);
}