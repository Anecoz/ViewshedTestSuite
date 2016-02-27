#pragma once
#include "Observer.h"
#include "KeyboardHandler.h"
#include "VoxelContainer.h"
#include "Camera.h"
#include <vector>
#include <GL\glew.h>
#include <GL\freeglut.h>

typedef std::vector<Observer> ObsList;
typedef std::vector<glm::vec3> VecList;

class VoxelViewshed
{
public:
	VoxelViewshed();
	~VoxelViewshed();

	void init();
	void render(glm::mat4 projMatrix, Camera* cam);
	VecList getPos();

	GLuint& getVoxelTexture(VoxelContainer&);	

	void tick(KeyboardHandler *);
private:
	// MEMBER VARIABLES

	// MEMBER OBJECTS
	//Observer observer; // just 1 for now
	ObsList obsList;

	// MEMBER METHODS

};

