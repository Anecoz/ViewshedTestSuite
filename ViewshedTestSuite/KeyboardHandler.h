#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>

class KeyboardHandler
{
public:
	KeyboardHandler();
	~KeyboardHandler() = default; // keyStates is on stack and automatically handleld

	// MEMBER METHODS
	bool isKeyDown(const unsigned char) const; // Check whether a key is down or nah

	void keyPressed(unsigned char); // Whenever a key gets pressed
	void keyUp(unsigned char); // Whenever a key is depressed

	void tick(); // Called every frame

	// MEMBER VARIABLES
	bool shouldMoveCamera = true;
	bool keyStates[256] = { false }; // For keeping track of which keys are currently pressed 

private:
	// Member variables	

	// Member functions
	void init(); // set OpenGL listening functions etc
};

