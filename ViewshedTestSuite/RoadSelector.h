#pragma once
#include "Terrain.h"
#include "DrawableModel.h"
#include "Road.h"
#include "Observer.h"
#include "SphericShadowmapViewshed.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>
#include <glm\glm.hpp>

typedef std::vector<Observer> ObsList;
typedef glm::vec3 Point;
typedef std::vector<Point> PointList;

/*
Class that should handle the road picking, should set observers once user has selected a road.
It should also be possible to drag a "already set" road point in order to change it.

Maybe this class should then hold the viewshed? makes sense...
*/
class RoadSelector
{
public:
	RoadSelector(Terrain*, SphericShadowmapViewshed*, DrawableModel*, Shader& shader);
	RoadSelector(Terrain*, DrawableModel*, Shader&);
	RoadSelector();
	~RoadSelector();

	void init(Terrain*, SphericShadowmapViewshed*, DrawableModel*, Shader&);
	void init(Terrain*, DrawableModel*, Shader&);

	void mouseDown(GLint button, GLint x, GLint y);
	void mouseUp(GLint button, GLint x, GLint y);

	void setPosTex(GLuint&);

	void render(glm::mat4& proj, glm::mat4 &camMatrix);

	PointList& getPointList();
	ObsList& getObservers();

private:
	// MEMBER OBJECTS
	PointList pointList;						// Container for the user selected road points (different from observers)
	Terrain *terrain;							// So that we can get the height at the mouse click
	SphericShadowmapViewshed *shadowViewshed = nullptr;	// To tell the viewshed to start calculating when we're building a road
	Road road;

	GLuint posTex = -1;		// Texture that holds positional encoding in its RGB-values
	GLfloat *posPixels;

	DrawableModel *pointModel;
	Shader shader;

	// MEMBER CONSTANTS
	const GLfloat MAX_SELECTION_DIST = 5.0;
	const GLfloat SCROLL_MOVE_DIST = 1.0;

	// MEMBER METHODS
	void movePointUp(Point&);
	void movePointDown(Point&);
	void startBuildRoad();
};

