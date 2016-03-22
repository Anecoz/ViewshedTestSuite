#include "Voxel.h"


Voxel::Voxel()
{
	this->pos = glm::vec3(0);
}

Voxel::Voxel(glm::vec3& posIn) {
	this->pos = posIn;
}

glm::vec3 Voxel::getPos() {
	return this->pos;
}

void Voxel::setPos(glm::vec3 posIn) {
	this->pos = posIn;
}