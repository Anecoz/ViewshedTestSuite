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

private:
	// MEMBER VARIABLES
	glm::vec3 pos;
	Shader shader;

	// MEMBER OBJECTS
	DrawableModel *model;
};

