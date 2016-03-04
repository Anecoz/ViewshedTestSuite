#pragma once
#include "Terrain.h"
#include "DrawableModel.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>
#include <glm\glm.hpp>

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

	void 

	PointList& getList();

private:
	// MEMBER OBJECTS
	PointList pointList;	// Container for the user selected road points (different from observers)
	Terrain *terrain;		// So that we can get the height at the mouse click

	GLuint posTex = -1;		// Texture that holds positional encoding in its RGB-values
	GLfloat *posPixels;

	DrawableModel *pointModel;
	Shader shader;

	// MEMBER CONSTANTS
	const GLfloat MAX_SELECTION_DIST = 5.0;
	const GLfloat SCROLL_MOVE_DIST = 1.0;

	// MEMBER METHODS
	void movePointUp(glm::vec3&);
	void movePointDown(glm::vec3&);

	bool startDrag(glm::vec3&);		// Return true if we started to drag a point, false otherwise
};

