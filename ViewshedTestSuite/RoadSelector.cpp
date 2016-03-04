#include "RoadSelector.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
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

RoadSelector::RoadSelector(Terrain* terrain, DrawableModel *simpleModel, Shader &shader) {	
	init(terrain, simpleModel, shader);
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

void RoadSelector::init(Terrain* terrain, DrawableModel *simpleModel, Shader &shader) {
	::instance = this;
	glutMouseFunc(::mouseFunc);
	this->terrain = terrain;
	this->pointModel = simpleModel;
	this->shader = shader;
	posPixels = (GLfloat*)malloc(sizeof(GLfloat) * 4 * Game::WINDOW_SIZE_X * Game::WINDOW_SIZE_Y);
}

PointList& RoadSelector::getList() {
	return pointList;
}

void RoadSelector::render(glm::mat4& projMatrix, glm::mat4& camMatrix) {
	shader.activate();
	glEnable(GL_DEPTH_TEST);

	for (glm::vec3 &point : pointList) {		
		pointModel->prepare();

		// Shader uploads
		shader.uploadMatrix(projMatrix, "projMatrix");
		shader.uploadMatrix(camMatrix, "camMatrix");

		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), point);
		shader.uploadMatrix(translationMatrix, "modelMatrix");
		pointModel->render();
	}	

	glDisable(GL_DEPTH_TEST);
	shader.deactivate();
}

void RoadSelector::mouseDown(GLint button, GLint x, GLint y) {
	/*
	0 = left click,
	1 = scroll wheel click,
	2 = right click,
	3 = scroll up,
	4 = scroll down
	*/
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

	switch (button) {
	case 0:
		// We want to be able to drag a point, check if any point is close to x,y,z
		if (!startDrag(glm::vec3(posX, posY, posZ)))
			pointList.push_back(glm::vec3(posX, posY + 3.0, posZ));
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		// Move any points that are at this world pos up
		movePointUp(glm::vec3(posX, posY, posZ));
		break;
	case 4:
		// Move any points that are at this world pos down
		movePointDown(glm::vec3(posX, posY, posZ));
		break;
	default:
		break;
	}
}

void RoadSelector::mouseUp(GLint button, GLint x, GLint y) {
	//printf("Mouse up! Button was %d, (x, y) = (%d, %d)\n", button, x, y);
}

bool RoadSelector::startDrag(glm::vec3 &posToCheck) {
	// Loop through the point list and check if anything is close
	for (glm::vec3 &point : pointList) {
		if (glm::distance(posToCheck, point) < MAX_SELECTION_DIST) {
			// Set this point to be dragging
			// TODO
			return true;
		}
	}
	return false;
}

void RoadSelector::movePointUp(glm::vec3 &posToCheck) {
	// First check if any point is close to the one where the user had the mouse
	// Loop through the point list and check distance
	for (glm::vec3 &point : pointList) {
		if (glm::distance(posToCheck, point) < MAX_SELECTION_DIST) {
			// Move the point
			point.y += SCROLL_MOVE_DIST;
			break;
		}
	}
}

void RoadSelector::movePointDown(glm::vec3 &posToCheck) {
	// Do same as for movePointUp
	for (glm::vec3 &point : pointList) {
		if (glm::distance(posToCheck, point) < MAX_SELECTION_DIST) {
			// Move the point
			point.y -= SCROLL_MOVE_DIST;
			break;
		}
	}
}