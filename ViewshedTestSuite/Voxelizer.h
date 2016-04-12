#include "Shader.h"
#include "DrawableModel.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "SVO.h"

#pragma once

/*
Responsible for voxelizing the scene, uses methods described in Nvidias article
*/
class Voxelizer
{
public:
	Voxelizer();
	~Voxelizer();

	void init(DrawableModel* terrainModel);

	GLuint& voxelize();	// Voxelizes the scene and returns the associated 3D texture
	GLint getVoxDim() { return voxDim; }
	SVO* getSVO() { return svo; }

	// MEMBER CONSTANTS
	static const GLuint WIDTH = 512;	// These need to be the same in order for the 3 orthographic projections...
	static const GLuint HEIGHT = 512;	// ... to work properly with the viewport
	static const GLuint DEPTH = 512;
private:
	// MEMBER OBJECTS
	Shader shader;
	GLuint voxelTex;
	DrawableModel *terrainModel;
	GLuint voxelCounterAtomicBuffer;

	GLuint voxelPosTex; // Texture that will hold (linearly) information about voxel positions
	GLuint voxelPosTBO; // Texture buffer object for voxel fragment list

	GLint voxDim;
	SVO* svo;

	// MEMBER METHODS
	void setupTexture();
	void calcVoxelList(GLboolean shouldStoreVoxels);
	GLuint getVoxelListSize();
	void genAtomicBuffer(GLuint&);
	void genLinearBuffer(GLint, GLenum, GLuint*, GLuint*);
	void buildSVO(GLuint*, GLuint);

	// MEMBER CONSTANTS
	// Some helpers for the orthographic projection matrices
	const GLfloat f_hWidth = (GLfloat)WIDTH / (GLfloat)2.0;
	const GLfloat f_Width = (GLfloat)WIDTH + 1.0f;
	const GLuint i_hWidth = WIDTH / 2;
	const glm::mat4 ortho = glm::ortho(-f_hWidth, f_hWidth, -f_hWidth, f_hWidth, 1.0f, f_Width);	// The orthographic projection matrices
	const glm::mat4 mvpX = ortho * glm::lookAt(glm::vec3(-1, i_hWidth, i_hWidth), glm::vec3(i_hWidth, i_hWidth, i_hWidth), glm::vec3(0, 1, 0));
	const glm::mat4 mvpZ = ortho * glm::lookAt(glm::vec3(i_hWidth, i_hWidth, -1), glm::vec3(i_hWidth, i_hWidth, i_hWidth), glm::vec3(0, 1, 0));
	const glm::mat4 mvpY = ortho * glm::lookAt(glm::vec3(i_hWidth, -1, i_hWidth), glm::vec3(i_hWidth, i_hWidth, i_hWidth), glm::vec3(0, 0, 1));
};

// DEBUG
//glBindTexture(GL_TEXTURE_3D, voxelTex);
//GLfloat *testArr = (GLfloat *)malloc(sizeof(GLfloat) * WIDTH * DEPTH * HEIGHT*4);

/*GLubyte *testArr = new GLubyte[WIDTH*HEIGHT*DEPTH];
printError("Before get text image");
glGetTextureImage(voxelTex, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, WIDTH*HEIGHT*DEPTH*sizeof(GLubyte), testArr);

printError("gettext image");
printf("texture at 100, 50, 100 is: %d\n", testArr[0]);

GLubyte max = 0;
for (int x = 0; x < 512; x++)
for (int y = 0; y < 128; y++)
for (int z = 0; z < 512; z++) {
GLubyte val = testArr[HEIGHT*WIDTH*z + WIDTH*y + x];
if (val > max) {
max = val;
printf("curr max is %d\n", max);
}
}
delete[](testArr);*/
