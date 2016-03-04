#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp> // Required for positions and such
#include "KeyboardHandler.h"

/*
This class should handle actual camera position but also the "look"-vector that depends on mouse
movement. Basically anything that is needed to get the camera matrix out when needed.
*/
class Camera
{
public:
	Camera(); // Default constructor
	Camera(const glm::vec3 initPos);
	~Camera(); // Needs to handle the cam instance pointer	

	// Needed for listening to mouse movements. glutPassiveMotionFunc must be set from a global context
	void handleMouseMovement(GLint, GLint);

	// Should be called once every frame
	void update(const KeyboardHandler*);

	// Returns a camera matrix to be used in shaders
	glm::mat4 getCameraMatrix() const;	

	// Where mouse is warped every warp
	static const int MOUSE_WARP_X = 200;
	static const int MOUSE_WARP_Y = 200;
	// Refresh rate for warping
	static const int WARP_REFRESH_RATE = 50;

	void updateTSinceLast();
	glm::vec2 getCurrMousePos();

	glm::vec3 getPos();

private:

	// MEMBER VARIABLES
	GLfloat sensitivity = 5.0f; // Higher value -> lower sensitivty....

	glm::vec2 currMousePos;

	glm::vec3 pos; // Position of the camera
	GLfloat tSinceLast; // Elapsed time since last time update was called

	// We need an up vector as well as a "look"-vector
	glm::vec3 up{ 0.0, 1.0, 0.0 };
	glm::vec3 look; // This one needs to be defined once we know the camera position, i.e. in constructor

	// MEMBER METHODS
	void init();
};

