#include "Observer.h"
#include <glm\gtc\matrix_transform.hpp>

extern "C" {
#include "GL_utilities.h"
}

Observer::Observer() {
	this->pos = glm::vec3(0, 0, 0);
	//init();
}

Observer::Observer(glm::vec3 pos, DrawableModel *simpleModel, Shader &simpleShader) {
	this->pos = pos;
	this->model = simpleModel;
	this->shader = simpleShader;
	//init();
}

Observer::~Observer() {
	model = nullptr;
}

void Observer::init() {

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

void Observer::render(glm::mat4& projMatrix, glm::mat4& camMatrix) {
	model->prepare();
	glEnable(GL_DEPTH_TEST);
	shader.activate();

	shader.uploadMatrix(projMatrix, "projMatrix");
	shader.uploadMatrix(camMatrix, "camMatrix");
	// Transformation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), this->pos);
	shader.uploadMatrix(translationMatrix, "modelMatrix");
	model->render();

	glDisable(GL_DEPTH_TEST);
	shader.deactivate();
}