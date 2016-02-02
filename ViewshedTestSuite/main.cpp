// To get rid of VS complaining that printf is not secure enough
#define _CRT_SECURE_NO_DEPRECATE

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <iostream>

#include "Camera.h"
#include "KeyboardHandler.h"
#include "Shader.h"

extern "C" //Ingemar's (useful) C headers
{
#include "GL_utilities.h"
}

using namespace std;

// Link
#pragma comment(lib, "glew32.lib")

// Initialize OpenGL and anything the software needs
void init() {
	dumpInfo();
	// GL inits
	glClearColor(0.2, 0.2, 0.5, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	printError("GL inits");

	// Other inits
	Camera* cam = new Camera(glm::vec3(0, 0, 0));
	KeyboardHandler* handler = new KeyboardHandler();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	//glutInitWindowSize(1024, 576); //16:9
	glutInitWindowSize(800, 600); //4:3
	glutCreateWindow("Viewshed test suite");
	//glutDisplayFunc(display);
	//glutTimerFunc(10, &timer, 0);

	glewExperimental = GL_TRUE; // A very tragic fix for things
	if (GLEW_OK != glewInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("glewInit failed, aborting.\n");
		exit(1);
	}
	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	init();
	glutMainLoop();	
	return 0;
}