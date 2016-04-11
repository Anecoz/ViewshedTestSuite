#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <string>
#include <vector>

#include "Shader.h"
#include "TerrainGenerator.h"
#include "VoxelContainer.h"
#include "DrawableModel.h"

typedef std::vector<glm::vec3> VecList;
class RoadSelector; // Forward declare

// Used to represent terrain, should have its own shader and all that jazz
// Also has own unique VAO and VBOs to store stuff
class Terrain
{
public:
	Terrain();
	~Terrain(); // Destructor needs to take care of freeing all VBOs

	void renderOrtho(glm::mat4 camMatrix, glm::mat4 projMatrix, glm::mat4 lightSpaceMatrix, GLuint& depthmap);
	void renderSpherical(glm::mat4, glm::mat4, GLuint&, VecList, GLfloat);
	void renderVoxelized(glm::mat4 camMatrix, glm::mat4 projMatrix, GLuint& voxTex, VecList lightArr, GLfloat height, GLint voxTexDim);

	void init(DrawableModel*); // Sets up shaders etc

	DrawableModel *getTerrainModel();

	GLuint& getEncodedPosTex(glm::mat4&, glm::mat4&, RoadSelector*);

	const GLuint getTriangleCount();
	VoxelContainer& getVoxels();

	// MEMBER CONSTANTS
	static const GLint TILE_SIZE = 512;
	static const GLint TERRAIN_HEIGHT = 128;

private:
	// MEMBER VARIABLES
	Shader orthoShader, sphericalShader, modelShader, voxelShader, encodePosShader;
	
	GLuint posFBO, posTex, depthBuffer;

	VoxelContainer voxels;

	const GLint VERTEX_COUNT = TILE_SIZE * TILE_SIZE;
	const GLuint TRIANGLE_COUNT = 2 * (TILE_SIZE - 1) * (TILE_SIZE - 1);

	// MEMBER OBJECTS
	TerrainGenerator generator;
	DrawableModel *terrainModel;
	DrawableModel *minimapModel;
	DrawableModel *simpleModel;

	// MEMBER METHODS
	void setupFBO();
	void setupModels();

	void renderPositionEncoding(glm::mat4&, glm::mat4&, RoadSelector*);

	void doRenderBoilerplate();
	void doPostRenderBoilerplate();
};

