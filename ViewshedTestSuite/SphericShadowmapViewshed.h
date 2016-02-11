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
	const GLuint SHADOW_WIDTH = 4096;
	const GLuint SHADOW_HEIGHT = 4096;
	GLuint depthMap, depthMapFBO;

	const glm::mat4 projMatrix = glm::ortho(-512.0f, 512.0f, -400.0f, 400.0f, 0.0f, 942.0f);
	const glm::mat4 lightView = glm::lookAt(glm::vec3(-430.0f, 256.0f, 256.0f), glm::vec3(256.0f, 0.0f, 256.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 lightSpaceMatrix = projMatrix * lightView;

	// MEMBER OBEJCTS
	Shader shader; // Special shader for getting depth map etc
	Terrain* terrain;

	// MEMBER METHODS
	void setupVAO();
	void setupFBO();

	void render(); // Make this private, because only getDepthMap is needed from outside
};

