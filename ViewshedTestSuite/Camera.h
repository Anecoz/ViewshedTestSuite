#pragma once
#include <glm\glm.hpp> // Required for positions and such

/*
This class should handle actual camera position but also the "look"-vector that depends on mouse
movement. Basically anything that is needed to get the camera matrix out when needed.
*/
class Camera
{
public:
	Camera(const glm::vec3 initPos);
	~Camera() = default; // Nothing special needs to be taken care of in here

	// Initializes things like mouse listening functions
	void init();

	// Should be called once every frame
	void update(bool keyStates[256]);

	// Returns a camera matrix to be used in shaders
	glm::mat4 getCameraMatrix();

	void onMouse(GLint, GLint); // Needed for listening to mouse movements. Static is needed

private:

	// MEMBER VARIABLES
	glm::vec3 pos; // Position of the camera
	GLfloat tSinceLast; // Elapsed time since last time update was called

	// We need an up vector as well as a "look"-vector
	glm::vec3 up{ 0.0, 1.0, 0.0 };
	glm::vec3 look; // This one needs to be defined once we know the camera position, i.e. in constructor

	// MEMBER METHODS
	
};

