#pragma once

#include "DrawableModel.h"
#include "Shader.h"
#include "Voxel.h"
#include "SVONode.h"
#include "VoxelContainer.h"
#include <GL\glew.h>
#include <vector>
#include <GL\freeglut.h>
#include "SVO.h"
#include <glm\glm.hpp>

typedef std::vector<Voxel> VoxelList;

class VoxelTester
{
public:
	VoxelTester();
	~VoxelTester();

	void init(); // Initializes the model and shader

	void createVoxelsFromTexture(GLuint&, GLint voxTexDim);
	void createVoxelsFromContainer(VoxelContainer&);
	void createVoxelsFromSVO(SVO*);

	void render(glm::mat4& projMatrix, glm::mat4& camMatrix);

private:
	// MEMBER METHODS
	void setupModel();
	void updateSVOChildren(SVONode*);

	// MEMBER OBJECTS
	DrawableModel *voxelModel;
	Shader shader;
	VoxelList voxelList;
};

