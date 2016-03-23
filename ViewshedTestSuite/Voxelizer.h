#include "Shader.h"
#include "DrawableModel.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#pragma once

/*
Responsible for voxelizing the scene, uses methods described in Nvidias article
*/
class Voxelizer
{
public:
	Voxelizer();
	~Voxelizer() = default;

	void init(DrawableModel* terrainModel);

	GLuint& voxelize();	// Voxelizes the scene and returns the associated 3D texture

	// MEMBER CONSTANTS
	static const GLuint WIDTH = 512;	// These need to be the same in order for the 3 orthographic projections...
	static const GLuint HEIGHT = 512;	// ... to work properly with the viewport
	static const GLuint DEPTH = 512;
private:
	// MEMBER OBJECTS
	Shader shader;
	GLuint voxelTex;
	DrawableModel *terrainModel;

	// MEMBER METHODS
	void setupTexture();

	// MEMBER CONSTANTS

	// Some helpers for the orthographic projection matrices
	const GLfloat f_hWidth = (GLfloat)WIDTH / (GLfloat)2.0;
	const GLfloat f_Width = (GLfloat)WIDTH;
	const GLuint i_hWidth = WIDTH / 2;
	const glm::mat4 ortho = glm::ortho(-f_hWidth, f_hWidth, -f_hWidth, f_hWidth, 1.0f, f_Width);	// The orthographic projection matrices
	const glm::mat4 mvpX = ortho * glm::lookAt(glm::vec3(-2, i_hWidth, i_hWidth), glm::vec3(i_hWidth, i_hWidth, i_hWidth), glm::vec3(0, 1, 0));
	const glm::mat4 mvpZ = ortho * glm::lookAt(glm::vec3(i_hWidth, i_hWidth, -2), glm::vec3(i_hWidth, i_hWidth, i_hWidth), glm::vec3(0, 1, 0));
	const glm::mat4 mvpY = ortho * glm::lookAt(glm::vec3(i_hWidth, -2, i_hWidth), glm::vec3(i_hWidth, i_hWidth, i_hWidth), glm::vec3(0, 0, -1));
};

