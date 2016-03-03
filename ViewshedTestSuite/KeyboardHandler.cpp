#include "KeyboardHandler.h"

// GLOBALS
KeyboardHandler *handlerInstance; // needed once again for setting listener functions
extern "C" {
	void onKeyUp(unsigned char key, GLint x, GLint y) {
		handlerInstance->keyUp(key);
	}
	void onKey(unsigned char key, GLint x, GLint y) {
		handlerInstance->keyPressed(key);
	}
}

// end GLOBALS


KeyboardHandler::KeyboardHandler()
{
	init(); // set listener functions and that sort of thing
}

void KeyboardHandler::init() {
	::handlerInstance = this;
	::glutKeyboardFunc(::onKey);
	::glutKeyboardUpFunc(::onKeyUp);
}

bool KeyboardHandler::isKeyDown(const unsigned char key) const {
	return keyStates[key];
}

void KeyboardHandler::keyPressed(unsigned char key) {
	if (key == 't')
		shouldMoveCamera = !shouldMoveCamera;
	keyStates[key] = true;
}

void KeyboardHandler::keyUp(unsigned char key) {
	keyStates[key] = false;
}

void KeyboardHandler::tick() {
	// Check if we're pressing escape, and in that case gracefully exit
	if (keyStates[27])
		exit(2);
}
