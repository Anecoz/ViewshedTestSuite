#include "Game.h"
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>

// GLOBALS, they are needed in order for OpenGL to play nicely with us
Game *gameInstance;

extern "C" {
	#include "GL_utilities.h"

	void timer(GLint i) {
		glutTimerFunc(Game::DISPLAY_UPDATE_RATE, &timer, i);
		glutPostRedisplay();
	}

	// Dummy
	void display() {
		gameInstance->tick();
	}
}
// end GLOBALS

Game::Game() {
	
}

Game::~Game() {
	delete camera;
	delete keyHandler;
	camera = nullptr;
	keyHandler = nullptr;
}

void Game::run(int& argc, char **argv) {
	// Pass on to init 
	init(argc, argv);
}

void Game::init(int& argc, char **argv) {
	gameInstance = this;	

	// Loads of boilerplate
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(INIT_WINDOW_POS, INIT_WINDOW_POS);
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutCreateWindow(WINDOW_TITLE.c_str());
	glutDisplayFunc(::display);
	glutTimerFunc(DISPLAY_UPDATE_RATE, ::timer, 0);

	// Test comment
	glewExperimental = GL_TRUE; // A very tragic fix for things
	if (GLEW_OK != glewInit())
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("glewInit failed, aborting.\n");		
		exit(1);
	}
	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	// GL inits
	dumpInfo(); // From ingemar, prints vendor and version number etc
	glClearColor(0.2, 0.2, 0.5, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	printError("GL inits");

	// Set the cam and keyhandler
	keyHandler = new KeyboardHandler();
	camera = new Camera();

	glutMainLoop();
}

void Game::tick() {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Do keyboard checks
	keyHandler->tick();
	// Update camera
	camera->update(keyHandler);

	// Swap buffers
	glutSwapBuffers();
}