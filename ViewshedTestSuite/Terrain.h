#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <string>

#include "Shader.h"
#include "TerrainGenerator.h"

// Used to represent terrain, should have its own shader and all that jazz
// Also has own unique VAO and VBOs to store stuff
class Terrain
{
public:
	Terrain();
	~Terrain(); // Destructor needs to take care of freeing all VBOs

	void renderOrtho(glm::mat4 camMatrix, glm::mat4 projMatrix, glm::mat4 lightSpaceMatrix, GLuint& depthmap);
	void renderSpherical(glm::mat4, glm::mat4, GLuint&, glm::vec3);

	void init(); // Sets up shaders etc
	void generate(); // Generates new terrain

	GLuint* getVertexVBO();
	GLuint* getIndexVBO();

	const GLuint getTriangleCount();

private:
	// MEMBER VARIABLES
	Shader orthoShader, sphericalShader;

	GLuint vao, vertexVBO, normalVBO, indexVBO;

	// These two hold raw data for the terrain. Free them using free()
	GLfloat *vertexArray;
	GLfloat *normalArray;
	GLuint *indexArray;

	const GLfloat TILE_SIZE = 512.0;
	const GLint VERTEX_COUNT = TILE_SIZE * TILE_SIZE;
	const GLuint TRIANGLE_COUNT = 2 * (TILE_SIZE - 1) * (TILE_SIZE - 1);

	const std::string VERTEX_IN_NAME = "inPosition";
	const std::string NORMAL_IN_NAME = "inNormal";

	const std::string VERTEX_FILE_NAME = "terrainvert.dat";
	const std::string NORMAL_FILE_NAME = "terrainnorm.dat";
	const std::string INDEX_FILE_NAME = "terrainind.dat";

	// MEMBER OBJECTS
	TerrainGenerator generator;

	// MEMBER METHODS
	void setupVAO();
	glm::vec3 calcNormal(GLfloat x, GLfloat y, GLfloat z); // Calculates a normal given a x,z position

	void activateDepthTexture();
	void doRenderBoilerplate();
	void doPostRenderBoilerplate();
};

