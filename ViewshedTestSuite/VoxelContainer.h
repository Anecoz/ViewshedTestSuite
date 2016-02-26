#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>

class VoxelContainer
{
public:
	VoxelContainer();
	~VoxelContainer();

	void init(); // sets all values to 0
	void setValue(GLuint, GLuint, GLuint, GLfloat);
	GLfloat getValue(GLuint, GLuint, GLuint);
	GLfloat* getArray();

	GLuint getDim();
	GLuint getHeight();

private:
	const GLuint DIM = 512;
	const GLuint HEIGHT = 128;

	GLfloat *arr;
};

