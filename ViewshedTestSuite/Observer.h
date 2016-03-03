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
	Observer(glm::vec3 pos);
	Observer();
	~Observer();
	Observer(const Observer&); // Copy constructor
	Observer& operator=(Observer other); // Copy assignment operator

	void render(glm::mat4 proj, Camera*);
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

	// MEMBER METHODS
	void setupModel();
};

