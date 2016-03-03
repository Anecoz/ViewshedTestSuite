#pragma once
#include "Observer.h"
#include "Terrain.h"
#include "DrawableModel.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>
#include <glm\glm.hpp>

typedef std::vector<Observer> ObsList;
typedef std::vector<glm::vec3> PointList;

/*
Class that should handle the road picking, should set observers once user has selected a road.
It should also be possible to drag a "already set" road point in order to change it.

Maybe this class should then hold the viewshed? makes sense...
*/
class RoadSelector
{
public:
	RoadSelector(Terrain*, DrawableModel*, Shader& shader);
	RoadSelector();
	~RoadSelector();

	void init(Terrain*, DrawableModel*, Shader&);

	void mouseDown(GLint button, GLint x, GLint y);
	void mouseUp(GLint button, GLint x, GLint y);

	void setPosTex(GLuint&);

	void render(glm::mat4& proj, glm::mat4 &camMatrix);

	PointList& getList();

private:
	// MEMBER OBJECTS
	ObsList obsList;		// Container for all observers
	PointList pointList;	// Container for the user selected road points (different from observers)
	Terrain *terrain;		// So that we can get the height at the mouse click

	GLuint posTex = -1;		// Texture that holds positional encoding in its RGB-values
	GLfloat *posPixels;

	DrawableModel *pointModel;
	Shader shader;

	// MEMBER METHODS
};

