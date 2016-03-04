#pragma once

#include <vector>
#include <glm\glm.hpp>
#include "Observer.h"

typedef std::vector<Observer> ObsList;
typedef std::vector<glm::vec3> PointList;

/*
Should hold a list of points and observers that represents a road.
Note that # of points is not necessarily the same as # of observers.
Should also hold some model for the road, i.e. a stretchable quad, that
connects the points into line-segments.

This class should __maybe__ hold the viewshed object... not sure yet.
*/
class Road
{
public:
	Road();
	~Road() = default;

	// MEMBER METHODS
	void addPoint(glm::vec3);
	void build();
	void render(glm::mat4 &proj, glm::mat4 &camMatrix);

private:
	// MEMBER OBJECTS
	PointList pointList;
	ObsList obsList;

};

