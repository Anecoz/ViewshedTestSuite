#pragma once
#include "Shader.h"
#include "Terrain.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

// Handles everything to do with viewshed implemented using spherical shadow mapping
class SphericShadowmapViewshed
{
public:
	SphericShadowmapViewshed();
	~SphericShadowmapViewshed();

	void init(Terrain* terrain);

	void setPos(glm::vec3);

	GLuint& getDepthMap();

	const glm::mat4 getLightSpaceMatrix();

private:
	// MEMBER VARIABLES
	GLuint vao;
	glm::vec3 pos;
	const GLuint SHADOW_WIDTH = 1024;
	const GLuint SHADOW_HEIGHT = 1024;
	GLuint depthMap, depthMapFBO;

	const glm::mat4 projMatrix = glm::ortho(-256.0f, 256.0f, -35.0f, 130.0f, 0.0f, 512.0f);
	const glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 30.0f, 256.0f), glm::vec3(256.0f, 0.0f, 256.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 lightSpaceMatrix = projMatrix * lightView;

	// MEMBER OBEJCTS
	Shader shader; // Special shader for getting depth map etc
	Terrain* terrain;

	// MEMBER METHODS
	void setupVAO();
	void setupFBO();

	void render(); // Make this private, because only getDepthMap is needed from outside
};

