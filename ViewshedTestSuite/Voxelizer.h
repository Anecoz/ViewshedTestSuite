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

	GLuint voxelize(glm::mat4& projMatrix, glm::mat4& camMatrix);	// Voxelizes the scene and returns the associated 3D texture

private:
	// MEMBER OBJECTS
	Shader shader;
	GLuint voxelTex;
	DrawableModel *terrainModel;

	// MEMBER METHODS
	void setupTexture();

	// MEMBER CONSTANTS
	const GLuint WIDTH = 512;
	const GLuint HEIGHT = 128;
	const GLuint DEPTH = 512;

	const glm::mat4 ortho = glm::ortho(-1, 1, -1, 1, 1, 3);	// The orthographic projection matrices
	const glm::mat4 mvpX = ortho * glm::lookAt(glm::vec3(2, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	const glm::mat4 mvpY = ortho * glm::lookAt(glm::vec3(0, 2, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
	const glm::mat4 mvpZ = ortho * glm::lookAt(glm::vec3(0, 0, 2), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
};

