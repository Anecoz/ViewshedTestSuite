#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>

class KeyboardHandler
{
public:
	KeyboardHandler();
	~KeyboardHandler() = default; // keyStates is on stack and automatically handleld

	bool isKeyDown(const unsigned char) const; // Check whether a key is down or nah

	void keyPressed(unsigned char); // Whenever a key gets pressed
	void keyUp(unsigned char); // Whenever a key is depressed

private:
	// Member variables
	bool keyStates[256] = { false }; // For keeping track of which keys are currently pressed 

	// Member functions
	void init(); // set OpenGL listening functions etc
};

