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

	GLuint voxelize();	// Voxelizes the scene and returns the associated 3D texture

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

	const glm::mat4 ortho = glm::ortho(-256.0f, 256.0f, -1.0f, 128.0f, 1.0f, 512.0f);	// The orthographic projection matrices
	const glm::mat4 mvpX = ortho * glm::lookAt(glm::vec3(-2, 0, 256), glm::vec3(256, 0, 256), glm::vec3(0, 1, 0));
	const glm::mat4 mvpZ = ortho * glm::lookAt(glm::vec3(256, 0, -2), glm::vec3(256, 0, 256), glm::vec3(0, 1, 0));

	const glm::mat4 orthoY = glm::ortho(-256.0f, 256.0f, -256.0f, 256.0f, 1.0f, 128.0f);
	const glm::mat4 mvpY = orthoY * glm::lookAt(glm::vec3(256, -2, 256), glm::vec3(256, 0, 256), glm::vec3(0, 0, -1));
	
};

