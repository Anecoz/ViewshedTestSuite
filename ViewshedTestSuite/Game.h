#pragma once
#include "Camera.h"
#include "KeyboardHandler.h"
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

private:
	// MEMBER OBJECTS
	Camera *camera;
	KeyboardHandler *keyHandler;

	// MEMBER VARIABLES
	const GLint WINDOW_SIZE_X = 800;
	const GLint WINDOW_SIZE_Y = 600;
	const std::string WINDOW_TITLE = "Viewshed test suite";
	const GLint INIT_WINDOW_POS = 500;

	// MEMBER METHODS
	void init(int&, char**); // handles all necessary initializations		
};

