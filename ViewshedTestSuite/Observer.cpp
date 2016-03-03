#include "Observer.h"
#include <glm\gtc\matrix_transform.hpp>

extern "C" {
#include "GL_utilities.h"
}

Observer::Observer() {
	this->pos = glm::vec3(0, 0, 0);
	//init();
}

Observer::Observer(glm::vec3 pos) {
	this->pos = pos;
	//init();
}

Observer::Observer(const Observer &other) {
	// Must create its own VAO and VBOs
	this->pos = other.pos;
	init();
}

Observer& Observer::operator=(Observer other) {
	this->pos = other.pos;
	init();

	return *this;
}

Observer::~Observer() {
	delete model;
	model = nullptr;
}

void Observer::init() {
	// Initialize the shader and setup the vao
	shader = Shader("observer.vert", "observer.frag");
	printError("after observer shader init");
	setupModel();
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
	model->prepare();
	glEnable(GL_DEPTH_TEST);
	shader.activate();

	shader.uploadMatrix(projMatrix, "projMatrix");
	shader.uploadMatrix(camera->getCameraMatrix(), "camMatrix");
	// Transformation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), this->pos);
	shader.uploadMatrix(translationMatrix, "modelMatrix");
	model->render();

	glDisable(GL_DEPTH_TEST);
	shader.deactivate();
}

void Observer::setupModel() {
	// Start with filling the arrays

	GLfloat *vertexArray = (GLfloat*)malloc(4 * 3 * sizeof(GLfloat));
	GLuint *indexArray = (GLuint*)malloc(2 * 3 * sizeof(GLuint));

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

	// init the model
	model = new DrawableModel(vertexArray, nullptr, indexArray);
	ShaderList shaders;
	shaders.push_back(shader);
	model->init(shaders, 4, 2);
}