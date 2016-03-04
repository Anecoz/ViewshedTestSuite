#include "Road.h"

Road::Road()
{
	// Dunno...
}

void Road::addPoint(glm::vec3 point) {
	pointList.push_back(point);
}

void Road::build() {
	// Well...
}

void Road::render(glm::mat4 &projMatrix, glm::mat4 &camMatrix) {
	// RoadSelector will render the points that are being made into roads, road will only render the line-segments when it's done
}