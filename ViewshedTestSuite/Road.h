#pragma once

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>
#include <glm\glm.hpp>
#include "Observer.h"
#include "DrawableModel.h"
#include "Shader.h"

typedef glm::vec3 Point;
typedef std::vector<Observer> ObsList;
typedef std::vector<Point> PointList;

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
	~Road();

	// MEMBER METHODS
	void addPoint(Point);
	void build(PointList); // Explicitily copy the list, so that we can discard it in the roadselector
	void render(glm::mat4 &proj, glm::mat4 &camMatrix);
	void init(DrawableModel*, Shader&); // Initialises shader and so on

	ObsList& getObservers();

private:
	// MEMBER VARIABLES
	GLuint vao, vertexVBO, indexVBO;
	GLfloat *vertexArray;
	GLuint *indexArray;
	GLint vertexCount;

	// MEMBER OBJECTS
	PointList pointList;
	ObsList obsList;
	Shader shader;

	DrawableModel *simpleModel;	// These two are for the observers
	Shader simpleShader;		// ditto

	// MEMBER METHODS
	void buildNewModel();
	void tessellate();
	void setupVAO();
};

