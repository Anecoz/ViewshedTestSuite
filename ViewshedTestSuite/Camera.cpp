#include "Camera.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm/gtx/transform.hpp>

Camera::Camera(const glm::vec3 initPos)
{
	// Initialize to some position provided in constructor
	this->pos = initPos;
	this->look = { pos.x + 1, pos.y, pos.z };

	// Initialize the tSinceLast to something
	this->tSinceLast = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}

// Handles updating of the camera, should be called once every frame draw
void Camera::update(bool keyStates[256])
{
	// I did the maths for this a loooooong 
	// time ago and sadly didn't comment anything.
	// It is what it is, move along.
	GLfloat timeElapsed = abs((GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000.0 - tSinceLast);
	tSinceLast = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	GLfloat speedModifier = 1.0;

	if (keyStates[32])
		speedModifier = 3.0;

	GLfloat speed = (speedModifier*timeElapsed)*100.0;
	// Directional vector
	glm::vec3 n = look - pos;
	n = glm::normalize(n);
	n = n * speed;

	glm::mat4 camtrans = glm::translate(glm::vec3(-pos.x, -pos.y, -pos.z));
	glm::mat4 invcamtrans = glm::translate(glm::vec3(pos.x, pos.y, pos.z));

	look = glm::vec3(camtrans * glm::vec4(look, 1.0f));
	glm::vec3 axis = glm::cross(look, up);
	look = glm::vec3(invcamtrans * glm::vec4(look, 1.0f));
	axis = glm::normalize(axis);
	axis = axis * speed;

	glm::vec3 tmp, tmp2, tmp3;

	tmp = pos;
	tmp2 = n;
	tmp3 = axis;

	if (keyStates['w'])
	{
		pos = tmp + tmp2;
		look = look + n;
	}
	else if (keyStates['s'])
	{
		pos = tmp - tmp2;
		look = look - n;
	}
	else if (keyStates['a'])
	{
		pos = tmp - tmp3;
		look = look - axis;
	}
	else if (keyStates['d'])
	{
		pos = tmp + tmp3;
		look = look + axis;
	}
	//printf("Now looking towards: %f %f %f\n", l.x, l.y, l.z);
}