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
	// Do nothing, should call Game::run later on
}

Game::~Game() {
	delete camera;
	delete keyHandler;
	delete roadSelector;
	delete simpleModel;
	simpleModel = nullptr;
	roadSelector = nullptr;
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
	glutInitWindowPosition(INIT_WINDOW_POS_X, INIT_WINDOW_POS_Y);
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutCreateWindow(WINDOW_TITLE.c_str());
	glutDisplayFunc(::display);
	glutTimerFunc(DISPLAY_UPDATE_RATE, ::timer, 0);

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
	printError("GL inits");

	// Set the objects
	roadSelector = new RoadSelector();
	keyHandler = new KeyboardHandler();
	camera = new Camera();

	simpleShader = Shader("observer.vert", "observer.frag");

	initSimpleModel();

	terrain.init(simpleModel);
	roadSelector->init(&terrain, simpleModel, simpleShader);
	roadSelector->setPosTex(terrain.getEncodedPosTex(camera->getCameraMatrix(), projMatrix, roadSelector));
	//shadowViewshed.initOrtho(&terrain);
	//shadowViewshed.initSpherical(&terrain, simpleModel, simpleShader);
	voxelViewshed.init(simpleModel, simpleShader);
	printError("after voxel init");
	voxTex = voxelViewshed.getVoxelTexture(terrain.getVoxels());
	printError("after voxel get text");

	glutMainLoop();
}

void Game::tick() {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Do keyboard checks
	keyHandler->tick();

	// Update camera
	camera->update(keyHandler);

	// Update the viewshed (steerable position)
	//shadowViewshed.tick(keyHandler);
	voxelViewshed.tick(keyHandler);
	printError("after voxel tick");

	// Get the shadow map
	//GLuint depthMap = shadowViewshed.getDepthMapOrtho();
	//GLuint depthMap = shadowViewshed.getDepthMapSpherical(projMatrix, camera);

	// Get the encoded position texture to be used in the roadselector
	GLuint posTex = terrain.getEncodedPosTex(camera->getCameraMatrix(), projMatrix, roadSelector);
	
	// Render observers
	voxelViewshed.render(projMatrix, camera);
	printError("after voxel draw");

	// Draw terrain
	//terrain.renderOrtho(camera->getCameraMatrix(), projMatrix, shadowViewshed.getOrthoLightSpaceMatrix(), depthMap);
	//terrain.renderSpherical(camera->getCameraMatrix(), projMatrix, depthMap, shadowViewshed.getPos(), shadowViewshed.getTargetHeight());
	terrain.renderVoxelized(camera->getCameraMatrix(), projMatrix, voxTex, voxelViewshed.getPos());

	// Render roads
	roadSelector->render(projMatrix, camera->getCameraMatrix());

	// Swap buffers
	glutSwapBuffers();
}

void Game::initSimpleModel() {

	GLfloat *vertexArray = (GLfloat*)malloc(4 * 3 * sizeof(GLfloat));
	GLuint *indexArray = (GLuint*)malloc(2 * 3 * sizeof(GLuint));

	// 4 vertices
	vertexArray[0] = -1.0;
	vertexArray[1] = 1.0;
	vertexArray[2] = 0.0;

	vertexArray[3] = -1.0;
	vertexArray[4] = -1.0;
	vertexArray[5] = 0.0;

	vertexArray[6] = 1.0;
	vertexArray[7] = 1.0;
	vertexArray[8] = 0.0;

	vertexArray[9] = 1.0;
	vertexArray[10] = -1.0;
	vertexArray[11] = 0.0;

	// Indices, triangle 1
	indexArray[0] = 0;
	indexArray[1] = 1;
	indexArray[2] = 2;

	// Triangle 2
	indexArray[3] = 2;
	indexArray[4] = 1;
	indexArray[5] = 3;

	// init the model
	simpleModel = new DrawableModel(vertexArray, nullptr, indexArray);
	ShaderList shaders;
	shaders.push_back(simpleShader);
	simpleModel->init(shaders, 4, 2);
}