#include "VoxelViewshed.h"

VoxelViewshed::VoxelViewshed()
{
	// call init
}

VoxelViewshed::~VoxelViewshed()
{

}

void VoxelViewshed::init(DrawableModel *simpleModel, Shader &simpleShader) {

	this->simpleModel = simpleModel;
	this->simpleShader = simpleShader;
}

void VoxelViewshed::addObserver(Point point) {
	obsList.push_back(Observer(point, simpleModel, simpleShader));
}

void VoxelViewshed::setObserverList(ObsList obsList) {
	this->obsList = obsList;
}

void VoxelViewshed::render(glm::mat4& projMatrix, glm::mat4& camMatrix) {
	for (Observer &obs : obsList) {
		obs.render(projMatrix, camMatrix);
	}
}

VecList VoxelViewshed::getPos() {
	VecList output;
	for (Observer &obs : obsList) {
		output.push_back(obs.getPos());
	}
	return output;
}

GLfloat VoxelViewshed::getTargetHeight() {
	return this->targetHeight;
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
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, WIDTH, HEIGHT, DEPTH, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, arr);

	// DEBUG
	/*GLubyte *testArr = (GLubyte *)malloc(sizeof(GLubyte) * WIDTH * DEPTH * HEIGHT);
	glGetTextureImage(voxTex, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, WIDTH*HEIGHT*DEPTH*sizeof(GLubyte), testArr);
	printf("texture at 7, 11, 178 is: %d\n", testArr[HEIGHT*WIDTH * 178 + DEPTH * 11 + 7]);

	int currmax = 0;
	for (int x = 0; x < 512; x++)
		for (int y = 0; y < 128; y++)
			for (int z = 0; z < 512; z++) {
				int val = testArr[HEIGHT*WIDTH*z + DEPTH*y + x];
				if (val > currmax) {
					currmax = val;
					printf("new max is %d\n", currmax);
				}
					
				//printf("texture at %d, %d, %d is %d\n", x, y, z, testArr[HEIGHT*WIDTH*z + DEPTH*y + x]);
			}*/

	return voxTex;
}

void VoxelViewshed::tick(KeyboardHandler* handler) {
	for (Observer &obs : obsList) {
		obs.tick(handler);
	}

	if (handler->keyStates['m']) {
		this->targetHeight += 0.2;
		printf("Target height is now %f\n", this->targetHeight);
	}
	else if (handler->keyStates['n']) {
		this->targetHeight -= 0.2;
		printf("Target height is now %f\n", this->targetHeight);
	}
}