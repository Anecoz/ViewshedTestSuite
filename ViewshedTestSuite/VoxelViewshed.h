#pragma once
#include "Observer.h"
#include "KeyboardHandler.h"
#include "VoxelContainer.h"
#include "Camera.h"
#include <GL\glew.h>
#include <GL\freeglut.h>

class VoxelViewshed
{
public:
	VoxelViewshed();
	~VoxelViewshed();

	void init();
	void render(glm::mat4 projMatrix, Camera* cam);
	glm::vec3 getPos();

	GLuint& getVoxelTexture(VoxelContainer&);	

	void tick(KeyboardHandler *);
private:
	// MEMBER VARIABLES

	// MEMBER OBJECTS
	Observer observer; // just 1 for now

	// MEMBER METHODS

};

