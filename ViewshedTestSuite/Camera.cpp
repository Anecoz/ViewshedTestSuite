#include "Camera.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm/gtx/transform.hpp>

// GLOBALS, they are needed in order for OpenGL to play nicely with us
Camera *camInstance;

extern "C"
void onMouse(GLint x, GLint y) {
	camInstance->handleMouseMovement(x, y);
}

// end GLOBALS

Camera::Camera(const glm::vec3 initPos)
{
	// Initialize to some position provided in constructor
	this->pos = initPos;
	this->look = { pos.x + 1, pos.y, pos.z };

	// Initialize state stuff
	init();	
}

void Camera::init() {
	::camInstance = this;
	glutPassiveMotionFunc(::onMouse);
	this->tSinceLast = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}

void Camera::handleMouseMovement(GLint x, GLint y) {
	GLfloat xdiff = ((GLfloat)200.0 - x) / 400.0; //Using offsets where mouse is warped everytime
	GLfloat ydiff = ((GLfloat)200.0 - y) / 400.0;

	// Rotate l point----------------
	//Y-axis----------------------
	glm::mat4 camtrans = glm::translate(glm::vec3(-pos.x, -pos.y, -pos.z));
	glm::mat4 camRotY = glm::rotate(xdiff, glm::vec3(0, 1, 0));
	glm::mat4 invcamtrans = glm::translate(glm::vec3(pos.x, pos.y, pos.z));
	glm::mat4 yTot = invcamtrans * camRotY * camtrans;
	look = glm::vec3(yTot * glm::vec4(look, 1.0));

	//X-axis (or whatever)-------------
	//needs check for if l is on up (y-axis)
	glm::mat4 camRotX = glm::mat4(1.0f);

	look = glm::vec3(camtrans * glm::vec4(look, 1.0));
	glm::vec3 axis = glm::cross(look, up);
	camRotX = glm::rotate(ydiff, axis);
	look = glm::vec3(camRotX * glm::vec4(look, 1.0));
	glm::vec3 axisAfter = glm::cross(look, up);
	if (axisAfter.x < 0.05 && axisAfter.x > -0.05
		&& axisAfter.z < 0.05 && axisAfter.z > -0.05
		&& axisAfter.y < 0.05 && axisAfter.y > -0.05)
	{
		camRotX = glm::rotate(-ydiff, axis);
		look = glm::vec3(camRotX * glm::vec4(look, 1.0));
	}
	look = glm::vec3(invcamtrans * glm::vec4(look, 1.0));
}

glm::mat4 Camera::getCameraMatrix() {
	// Glm provides a nice look-at implementation here
	return glm::lookAt(this->pos, this->look, this->up);
}

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