#pragma once

#include "DrawableModel.h"
#include "Shader.h"
#include "Voxel.h"
#include <GL\glew.h>
#include <vector>
#include <GL\freeglut.h>
#include <glm\glm.hpp>

typedef std::vector<Voxel> VoxelList;

class VoxelTester
{
public:
	VoxelTester();
	~VoxelTester();

	void init(); // Initializes the model and shader

	void createVoxelsFromTexture(GLuint&);

	void render(glm::mat4& projMatrix, glm::mat4& camMatrix);

private:
	// MEMBER METHODS
	void setupModel();

	// MEMBER OBJECTS
	DrawableModel *voxelModel;
	Shader shader;
	VoxelList voxelList;
};

