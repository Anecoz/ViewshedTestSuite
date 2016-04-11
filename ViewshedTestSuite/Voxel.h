#pragma once

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GL\freeglut.h>

class Voxel
{
public:
	Voxel();
	Voxel(glm::vec3&, GLint size, GLboolean empty);
	~Voxel() = default;

	glm::vec3 getPos();
	void setPos(glm::vec3);

	GLint getSize() { return this->size; }

	GLboolean getEmpty() { return this->empty; }

private:
	glm::vec3 pos;
	GLint size;
	GLboolean empty;
};