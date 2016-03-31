#pragma once
#include "Observer.h"
#include "KeyboardHandler.h"
#include "VoxelContainer.h"
#include "Camera.h"
#include "DrawableModel.h"
#include "Shader.h"
#include <vector>
#include <GL\glew.h>
#include <GL\freeglut.h>

typedef glm::vec3 Point;
typedef std::vector<Observer> ObsList;
typedef std::vector<glm::vec3> VecList;

class VoxelViewshed
{
public:
	VoxelViewshed();
	~VoxelViewshed();

	void init(DrawableModel *simpleModel, Shader &simpleShader);
	void render(glm::mat4& projMatrix, glm::mat4 &camMatrix);
	VecList getPos();
	GLfloat getTargetHeight();

	void addObserver(Point);
	void setObserverList(ObsList);

	GLuint& getVoxelTexture(VoxelContainer&);	

	void tick(KeyboardHandler *);
private:
	// MEMBER VARIABLES
	DrawableModel *simpleModel;
	Shader simpleShader;
	GLfloat targetHeight = 2.0;

	// MEMBER OBJECTS
	ObsList obsList;

	// MEMBER METHODS

};

