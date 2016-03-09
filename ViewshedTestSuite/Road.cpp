#include "Road.h"
#include <glm/gtc/matrix_transform.hpp>

Road::Road()
{
	
}

Road::~Road() {
	free(vertexArray);
	free(indexArray);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &indexVBO);
}

void Road::init(DrawableModel *simpleModel, Shader &simpleShader) {
	shader = Shader("observer.vert", "observer.frag");
	this->simpleModel = simpleModel;
	this->simpleShader = simpleShader;
}

void Road::addPoint(Point point) {
	pointList.push_back(point);
}

void Road::build(PointList pointList) {
	this->pointList = pointList;

	// Build a model that holds the points, draw them using GL_LINE_STRIP
	buildNewModel();

	// Tessellate into observers
	tessellate();
}

void Road::render(glm::mat4 &projMatrix, glm::mat4 &camMatrix) {
	// RoadSelector will render the points that are being made into roads, road will only render the line-segments when it's done
	glEnable(GL_DEPTH_TEST);
	shader.activate();
	glBindVertexArray(vao);

	// Uploads
	shader.uploadMatrix(projMatrix, "projMatrix");
	shader.uploadMatrix(camMatrix, "camMatrix");
	shader.uploadMatrix(glm::translate(glm::mat4(1.0f), Point(0)), "modelMatrix");

	// Draw call
	glDrawElements(GL_LINE_STRIP, vertexCount, GL_UNSIGNED_INT, 0L);

	glDisable(GL_DEPTH_TEST);
	shader.deactivate();
	glBindVertexArray(0);

	/*for (Observer &observer : obsList) {
		observer.render(projMatrix, camMatrix);
	}*/
}

ObsList& Road::getObservers() {
	return obsList;
}

void Road::tessellate() {
	// Clear previous list
	ObsList().swap(obsList);

	// For now, just make 1 observer for each point in pointlist
	// In the future, might want to make sure that for example the highest point is always represented.
	for (Point &point : pointList) {
		obsList.push_back(Observer(point, simpleModel, simpleShader));
	}
}

void Road::buildNewModel() {
	// Free old stuff first (in case there is somethign, else we lose the handle)
	free(vertexArray);
	free(indexArray);

	// Create a vertexarray and indexarray using the points in our list
	vertexCount = pointList.size();

	vertexArray = (GLfloat*)malloc(sizeof(GLfloat) * vertexCount * 3);
	indexArray = (GLuint*)malloc(sizeof(GLuint) * vertexCount);

	// Loop trough all points and set the vertexarray accordingly
	int counter = 0;
	for (Point &point : pointList) {
		vertexArray[counter*3 + 0] = point.x;
		vertexArray[counter*3 + 1] = point.y;
		vertexArray[counter*3 + 2] = point.z;

		// indexarray
		indexArray[counter] = counter; // Index just corresponds to the counter

		counter++;
	}

	setupVAO();
}

void Road::setupVAO() {
	// Clear all previous things first
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &indexVBO);

	// Now generate the new stuff
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &indexVBO);

	glBindVertexArray(vao);
	shader.activate();

	// Vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);

	shader.setAndEnableVertexAttrib("inPosition");
	
	// Index VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexCount*sizeof(GLuint), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
}