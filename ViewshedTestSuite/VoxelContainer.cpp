#include "VoxelContainer.h"


VoxelContainer::VoxelContainer() {

}

VoxelContainer::~VoxelContainer() {
	free(arr);
}

void VoxelContainer::init() {
	// Loop through array and set to 0
	arr = (GLfloat *)malloc(sizeof(GLfloat) * DIM * DIM * HEIGHT);
	for (int x = 0; x < DIM; x++)
		for (int z = 0; z < DIM; z++)
			for (int y = 0; y < HEIGHT; y++) {
				arr[HEIGHT*DIM*z + DIM*y + x] = 0.0;
			}
}

void VoxelContainer::setValue(GLuint x, GLuint y, GLuint z, GLfloat value) {
	arr[HEIGHT*DIM*z + DIM*y + x] = value;
}

GLfloat VoxelContainer::getValue(GLuint x, GLuint y, GLuint z) {
	return arr[HEIGHT*DIM*z + DIM*y + x];
}

GLuint VoxelContainer::getDim() { return DIM; };
GLuint VoxelContainer::getHeight() { return HEIGHT; };

GLfloat *VoxelContainer::getArray() {
	return arr;
}
