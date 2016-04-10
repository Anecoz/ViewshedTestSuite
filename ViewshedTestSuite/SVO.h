#include <vector>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include "SVONode.h"

//typedef std::vector<SVONode> NodeList;

#pragma once
/*
Handles abstraction of the octree.
*/

class SVO
{
public:
	SVO(GLint startSize, glm::vec3 startPos);
	~SVO() = default;

	// METHODS
	GLint insert(glm::vec3& voxelPos);	// Inserts a voxel at the correct place in the octree

private:
	// CONSTANTS
	const GLuint TARGET_SIZE = 1;

	// OBJECTS
	//NodeList nodeList;
	SVONode topNode;

	// METHODS

};

