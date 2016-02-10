#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <string>

#include "Shader.h"

// Used to represent terrain, should have its own shader and all that jazz
// Also has own unique VAO and VBOs to store stuff
class Terrain
{
public:
	Terrain();
	~Terrain(); // Destructor needs to take care of freeing all VBOs

	void render(glm::mat4 camMatrix, glm::mat4 projMatrix); // Takes camera matrix

	void init(); // Sets up shaders etc
	void generate(); // Generates new terrain

private:
	// MEMBER VARIABLES
	Shader shader;

	GLuint vao, vertexVBO, indexVBO;

	// These two hold raw data for the terrain. Free them using free()
	GLfloat *vertexArray;
	GLuint *indexArray; 

	const GLfloat TILE_SIZE = 512.0;
	const GLint VERTEX_COUNT = TILE_SIZE * TILE_SIZE;
	const GLuint TRIANGLE_COUNT = 2 * (TILE_SIZE - 1) * (TILE_SIZE - 1);

	const std::string VERTEX_IN_NAME = "inPosition";

	// MEMBER METHODS
	void setupVAO();
};

