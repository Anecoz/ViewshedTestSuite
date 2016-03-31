#pragma once
#include "Camera.h"
#include "KeyboardHandler.h"
#include "Terrain.h"
#include "SphericShadowmapViewshed.h"
#include "VoxelViewshed.h"
#include "RoadSelector.h"
#include "DrawableModel.h"
#include "Shader.h"
#include "Voxelizer.h"
#include "VoxelTester.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <string>

// Should handle the main control of the loop, i.e. setting all the inits and setting the display func timers etc.
class Game
{
public:
	//Game();
	Game(); // For passing directly from main
	~Game();

	void run(int&, char**);

	static const int DISPLAY_UPDATE_RATE = 10;

	void tick(); // per-frame tick.

	static const GLint WINDOW_SIZE_X = 800;
	static const GLint WINDOW_SIZE_Y = 800;

private:
	// MEMBER OBJECTS
	Camera *camera;
	KeyboardHandler *keyHandler;
	RoadSelector *roadSelector;
	DrawableModel *simpleModel;
	Shader simpleShader;
	Voxelizer voxelizer;
	VoxelTester voxelTester;

	// Terrain object
	Terrain terrain;

	// VIEWSHED
	//SphericShadowmapViewshed shadowViewshed;
	VoxelViewshed voxelViewshed;
	GLuint voxTex; // Get this once, and not every frame

	// Projection matrix
	glm::mat4 projMatrix = glm::perspective(45.0f, 1.0f / 1.0f, 0.1f, 2000.0f);

	// MEMBER VARIABLES	
	const std::string WINDOW_TITLE = "Viewshed test suite";
	const GLint INIT_WINDOW_POS_X = 500;
	const GLint INIT_WINDOW_POS_Y = 0;

	// MEMBER METHODS
	void init(int&, char**); // handles all necessary initializations

	void initSimpleModel();
};

