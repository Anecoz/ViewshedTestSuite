#include "RoadSelector.h"
#include "Game.h"
#include <stdio.h>

// GLOBALS
RoadSelector *instance; // needed once again for setting listener functions
extern "C" {
	void mouseFunc(GLint button, GLint state, GLint x, GLint y) {
		if (state == GLUT_DOWN)
			instance->mouseDown(button, x, y);
		else if (state == GLUT_UP)
			instance->mouseUp(button, x, y);
	}
}

// end GLOBALS

RoadSelector::RoadSelector()
{
	terrain = nullptr;
}

RoadSelector::RoadSelector(Terrain* terrain) {
	init(terrain);
}

RoadSelector::~RoadSelector() {
	delete instance;
	instance = nullptr;
	free(posPixels);
	posPixels = nullptr;
}

void RoadSelector::setPosTex(GLuint& tex) {
	posTex = tex;
}

void RoadSelector::init(Terrain* terrain) {
	::instance = this;
	glutMouseFunc(::mouseFunc);
	this->terrain = terrain;
	posPixels = (GLfloat*)malloc(sizeof(GLfloat) * 4 * Game::WINDOW_SIZE_X * Game::WINDOW_SIZE_Y);
}

void RoadSelector::mouseDown(GLint button, GLint x, GLint y) {
	// Button 0 = left click, 2 = right click etc.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, posTex);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, posPixels);

	// Get the pixel at mouse x and y
	// Texture has the origin in lower left, mouse coords origin in upper left... fix the y
	y = Game::WINDOW_SIZE_Y - y;
	GLfloat r = posPixels[3 * (x + Game::WINDOW_SIZE_X*y) + 0];
	GLfloat g = posPixels[3 * (x + Game::WINDOW_SIZE_X*y) + 1];
	GLfloat b = posPixels[3 * (x + Game::WINDOW_SIZE_X*y) + 2];

	GLfloat posX = r*512.0;
	GLfloat posY = g*128.0;
	GLfloat posZ = b*512.0;

	printf("Clicked! World position is %f, %f, %f\n", posX, posY, posZ);
	//printf("Color at click is %f, %f, %f\n", r, g, b);

	if (button == 0 && !isSelecting) {
		isSelecting = true;
	}
	else if (button == 2 && isSelecting) {
		isSelecting = false;
		// Begin tessellation
	}
}

void RoadSelector::mouseUp(GLint button, GLint x, GLint y) {
	//printf("Mouse up! Button was %d, (x, y) = (%d, %d)\n", button, x, y);
}
