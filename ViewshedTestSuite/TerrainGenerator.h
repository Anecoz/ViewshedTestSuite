#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include "VoxelContainer.h"

// Responsible for generating procedural terrain, uses FBM
class TerrainGenerator
{
public:
	TerrainGenerator();
	~TerrainGenerator() = default;

	void generate(VoxelContainer *voxels, GLfloat* vertexArr, GLfloat* normalArr, GLuint* indexArr, GLint tileSize, GLuint triCount, GLuint vertCount);
	
	void setSeed(GLfloat seedIn);

private:
	glm::mat2 rotate2D;
	GLfloat seed, MOD2x, MOD2y, MOD3x, MOD3y, MOD3z;

	// MEMBER METHODS
	GLfloat Hash12(GLfloat px, GLfloat py) const;
	GLfloat Noise1(const GLfloat x, const GLfloat y) const;
	GLfloat calcNeutral(GLfloat posX, GLfloat posY) const;
	glm::vec3 calcNormal(GLfloat x, GLfloat y, GLfloat z);
	GLfloat getHeight(const GLfloat x, const GLfloat z) const;
};

