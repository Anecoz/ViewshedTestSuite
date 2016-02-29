#include "VoxelViewshed.h"

VoxelViewshed::VoxelViewshed()
{
	// call init
}

VoxelViewshed::~VoxelViewshed()
{

}

void VoxelViewshed::init() {

	// Fill the observer list with observers
	int n = 5;
	glm::vec3 startPos = { 256, 20, 256 };
	int counter = 0;
	for (int i = 0; i < n; i++) {
		Observer currObs(startPos + glm::vec3(counter, 0, 0));
		obsList.push_back(currObs);
		counter += 10;
	}
}

void VoxelViewshed::render(glm::mat4 projMatrix, Camera* camera) {
	for (Observer &obs : obsList) {
		obs.render(projMatrix, camera);
	}
}

VecList VoxelViewshed::getPos() {
	//return observer.getPos();
	VecList output;
	for (Observer &obs : obsList) {
		output.push_back(obs.getPos());
	}
	return output;
}

GLuint& VoxelViewshed::getVoxelTexture(VoxelContainer& voxels) {
	GLubyte *arr = voxels.getArray();

	const GLuint WIDTH = voxels.getDim();
	const GLuint HEIGHT = voxels.getHeight();
	const GLuint DEPTH = WIDTH;

	// Generate a 3D texture
	GLuint voxTex;
	
	glGenTextures(1, &voxTex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, WIDTH, HEIGHT, DEPTH, 0, GL_RED_INTEGER,
		GL_UNSIGNED_BYTE, arr);

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
	for (Observer &obs : obsList) {
		obs.tick(handler);
	}
}