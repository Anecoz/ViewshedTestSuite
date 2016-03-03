#pragma once
#include "Shader.h"
#include "Terrain.h"
#include "KeyboardHandler.h"
#include "Camera.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Observer.h"
#include "DrawableModel.h"

// Handles everything to do with viewshed implemented using spherical shadow mapping
// Also features an implementation of using orthographic shadow mapping (directional)
class SphericShadowmapViewshed
{
public:
	SphericShadowmapViewshed();
	~SphericShadowmapViewshed();

	void initOrtho(Terrain* terrain);
	void initSpherical(Terrain* terrain);

	glm::vec3 getPos();

	GLuint& getDepthMapOrtho();
	GLuint& getDepthMapSpherical(glm::mat4, Camera*);

	const glm::mat4 getOrthoLightSpaceMatrix();

	void tick(KeyboardHandler*);

	GLfloat getTargetHeight() const;

	// Maximum "view" distance for the viewshed
	static const GLint VIEWSHED_MAX_DIST = 256;

private:
	// MEMBER VARIABLES
	GLuint vao;
	const GLuint SHADOW_WIDTH = 4096;
	const GLuint SHADOW_HEIGHT = 4096;
	GLuint depthMap, depthMapFBO;

	GLfloat targetHeight;

	// For the orthogonal projection
	const glm::mat4 orthoProjMatrix = glm::ortho(-512.0f, 512.0f, -400.0f, 400.0f, 0.0f, 942.0f);
	const glm::mat4 orthoLightView = glm::lookAt(glm::vec3(-430.0f, 256.0f, 256.0f), glm::vec3(256.0f, 0.0f, 256.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 orthoLightSpaceMatrix = orthoProjMatrix * orthoLightView;

	// MEMBER OBJECTS
	Shader shader; // Special shader for getting depth map etc
	Terrain* terrain;
	Observer observer;
	DrawableModel *terrainModel;

	// MEMBER METHODS
	void setupModel();
	void setupFBO();

	void renderOrtho(); // Make this private, because only getDepthMap is needed from outside
	void renderSpherical(glm::mat4, Camera*); // ditto, also needs proj and cam matrix for rendering the model
	void doRenderBoilerplate(); // Some generic code that must be done either way
	void doPostRenderBoilerplate(); // ditto but post-render
};

