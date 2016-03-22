#pragma once

#include <glm\glm.hpp>

class Voxel
{
public:
	Voxel();
	Voxel(glm::vec3&);
	~Voxel() = default;

	glm::vec3 getPos();
	void setPos(glm::vec3);

private:
	glm::vec3 pos;
};