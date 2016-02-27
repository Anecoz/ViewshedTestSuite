#include "Observer.h"
#include <glm\gtc\matrix_transform.hpp>

extern "C" {
#include "GL_utilities.h"
}

Observer::Observer() {
	this->pos = glm::vec3(0, 0, 0);
	vertexArray = nullptr;
	indexArray = nullptr;
	vao = 0;
	vertexVBO = 0;
	indexVBO = 0;
	init();
}

Observer::Observer(glm::vec3 pos) {
	this->pos = pos;
	vertexArray = nullptr;
	indexArray = nullptr;
	vao = 0;
	vertexVBO = 0;
	indexVBO = 0;
	init();
}

Observer::Observer(const Observer &other) {
	// Must create its own VAO and VBOs
	vertexArray = nullptr;
	indexArray = nullptr;
	vao = 0;
	vertexVBO = 0;
	indexVBO = 0;
	this->pos = other.pos;
	init();
}

Observer& Observer::operator=(Observer other) {
	vertexArray = nullptr;
	indexArray = nullptr;
	this->pos = other.pos;
	init();

	return *this;
}

Observer::~Observer() {
	if (vertexArray != nullptr)
		free(vertexArray);
	if (indexArray != nullptr)
		free(indexArray);
	if (vao != 0)
		glDeleteVertexArrays(1, &vao);
	if (vertexVBO != 0)
		glDeleteBuffers(1, &vertexVBO);
	if (indexVBO != 0)
		glDeleteBuffers(1, &indexVBO);
}

void Observer::init() {
	// Initialize the shader and setup the vao
	shader = Shader("observer.vert", "observer.frag");
	printError("after observer shader init");
	setupVAO();
	printError("after observer vao setup");
}

void Observer::setPos(glm::vec3 newPos) {
	this->pos = newPos;
}

glm::vec3 Observer::getPos() {
	return this->pos;
}

void Observer::tick(KeyboardHandler *handler) {
	GLfloat velocity = 1.0;

	if (handler->keyStates['i'])
		this->pos.x += velocity;
	else if (handler->keyStates['k'])
		this->pos.x -= velocity;
	else if (handler->keyStates['j'])
		this->pos.z += velocity;
	else if (handler->keyStates['l'])
		this->pos.z -= velocity;
	else if (handler->keyStates['o'])
		this->pos.y += velocity;
	else if (handler->keyStates['u'])
		this->pos.y -= velocity;
}

void Observer::render(glm::mat4 projMatrix, Camera *camera) {
	glBindVertexArray(vao);
	glEnable(GL_DEPTH_TEST);
	shader.activate();

	shader.uploadMatrix(projMatrix, "projMatrix");
	shader.uploadMatrix(camera->getCameraMatrix(), "camMatrix");
	// Transformation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), this->pos);
	shader.uploadMatrix(translationMatrix, "modelMatrix");
	glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * 3, GL_UNSIGNED_INT, 0L);

	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
	shader.deactivate();
}

void Observer::setupVAO() {
	// Start with filling the arrays

	vertexArray = (GLfloat*)malloc(VERTEX_COUNT * 3 * sizeof(GLfloat));
	indexArray = (GLuint*)malloc(TRIANGLE_COUNT * 3 * sizeof(GLuint));

	// 4 vertices
	vertexArray[0] = -1.0;
	vertexArray[1] = 1.0;
	vertexArray[2] = 0.0;

	vertexArray[3] = -1.0;
	vertexArray[4] = -1.0;
	vertexArray[5] = 0.0;

	vertexArray[6] = 1.0;
	vertexArray[7] = 1.0;
	vertexArray[8] = 0.0;

	vertexArray[9] = 1.0;
	vertexArray[10] = -1.0;
	vertexArray[11] = 0.0;

	// Indices, triangle 1
	indexArray[0] = 0;
	indexArray[1] = 1;
	indexArray[2] = 2;

	// Triangle 2
	indexArray[3] = 2;
	indexArray[4] = 1;
	indexArray[5] = 3;

	// Fill up the VAO
	shader.activate();
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &indexVBO);

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * 3 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);
	shader.setAndEnableVertexAttrib("inPosition");
	shader.deactivate();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLE_COUNT * 3 * sizeof(GLuint), indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
}