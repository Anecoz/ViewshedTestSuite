#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <vector>

#pragma once
class SVONode;
typedef std::vector<SVONode*> Children;

/*
Represents a single node in SVO, holds its 8 children
*/
class SVONode
{
public:
	SVONode();
	SVONode(GLint initSize, glm::vec3 initPos);		// Pos is center of this volume
	~SVONode();

	GLuint getOctant(glm::vec3&);					// Returns what octant a position falls into
	GLboolean isInside(glm::vec3&);

	void split();

	GLboolean isLeaf = true;
	GLboolean containsVoxel = false;
	Children children;
	GLint size;

private:
	glm::vec3 pos;

	GLint minX, minY, minZ;
	GLint maxX, maxY, maxZ;

	glm::vec3 calcChildPos(GLint octant);
};

