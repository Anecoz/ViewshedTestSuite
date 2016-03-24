#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include "Shader.h"
#include "Camera.h"
#include "KeyboardHandler.h"
#include "DrawableModel.h"

// Abstraction for observer, includes a model and position and its own shader
class Observer
{
public:
	Observer(glm::vec3 pos, DrawableModel *simpleModel, Shader &simpleShader);
	Observer();
	~Observer();

	void render(glm::mat4& proj, glm::mat4& camMatrix);
	void init();

	void setPos(glm::vec3);
	glm::vec3 getPos();

	void tick(KeyboardHandler*);

	void setColor(glm::vec3);

private:
	// MEMBER VARIABLES
	glm::vec3 pos;
	Shader shader;
	glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);

	// MEMBER OBJECTS
	DrawableModel *model;
};

