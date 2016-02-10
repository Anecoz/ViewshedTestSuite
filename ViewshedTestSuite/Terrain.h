#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>

#include "Shader.h"

// Used to represent terrain, should have its own shader and all that jazz
// Also has own unique VAO and VBOs to store in them
class Terrain
{
public:
	Terrain();
	~Terrain(); // Destructor needs to take care of freeing all VBOs

	void render(glm::mat4 camMatrix); // Takes camera matrix

	void generate(); // Generates a terrain object

private:
	Shader shader;
	GLuint vao;

};

