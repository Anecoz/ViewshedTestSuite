#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include "SVO.h"
#include "SVONode.h"

#pragma once 
/*
Handles conversion from CPU SVO representation to a 3D texture usable by the GPU.
*/
class SVOConverter
{
public:
	SVOConverter();
	~SVOConverter() = default;

	// METHODS
	GLuint& buildTexture(SVO*, GLint numNodes);

private:
	// CONSTANTS
	const GLint NUM_CHILDREN_PER_NODE = 8;

	// VARIABLES
	GLuint texId;	// 3D texture handle

	GLint dim;
	glm::vec2 currentTexturePos{ 0, 0 };

	// METHODS
	glm::vec2 getNextAvailableCoord();
	void insertInArr(GLuint*, GLuint val, GLint x, GLint y, GLint z, GLint channel);

	void insertChildrenInTexture(GLuint* data, SVONode* node, glm::vec2 nodePos);
};

