#pragma once
#include "Camera.h"
#include "KeyboardHandler.h"
#include "Terrain.h"
#include "SphericShadowmapViewshed.h"
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

	static const int DISPLAY_UPDATE_RATE = 20;

	void tick(); // per-frame tick.

	static const GLint WINDOW_SIZE_X = 1280;
	static const GLint WINDOW_SIZE_Y = 1280;

private:
	// MEMBER OBJECTS
	Camera *camera;
	KeyboardHandler *keyHandler;

	// Temporary terrain object
	Terrain terrain;

	// VIEWSHED
	SphericShadowmapViewshed viewshed;

	// Projection matrix
	const glm::mat4 projMatrix = glm::perspective(45.0f, 1.0f / 1.0f, 0.1f, 2000.0f);

	// MEMBER VARIABLES	
	const std::string WINDOW_TITLE = "Viewshed test suite";
	const GLint INIT_WINDOW_POS_X = 500;
	const GLint INIT_WINDOW_POS_Y = 0;

	// MEMBER METHODS
	void init(int&, char**); // handles all necessary initializations		
};

