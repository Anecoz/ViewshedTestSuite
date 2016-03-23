#pragma once
#include "Shader.h"
#include "Terrain.h"
#include "KeyboardHandler.h"
#include "Camera.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <vector>
#include "Observer.h"
#include "DrawableModel.h"

typedef std::vector<Observer> ObsList;
typedef std::vector<glm::vec3> VecList;

// Handles everything to do with viewshed implemented using spherical shadow mapping
// Also features an implementation of using orthographic shadow mapping (directional)
class SphericShadowmapViewshed
{
public:
	SphericShadowmapViewshed();
	~SphericShadowmapViewshed();

	void initOrtho(Terrain* terrain);
	void initSpherical(Terrain* terrain, DrawableModel*, Shader&);

	VecList getPos();

	GLuint& getDepthMapOrtho();
	GLuint& getDepthMapSpherical(glm::mat4, Camera*);
	GLuint& get3DDepthMap(glm::mat4, Camera*);

	const glm::mat4 getOrthoLightSpaceMatrix();

	void tick(KeyboardHandler*);

	GLfloat getTargetHeight() const;

	void setObserverList(ObsList);

	// Maximum "view" distance for the viewshed
	static const GLint VIEWSHED_MAX_DIST = 256;
	// Maximum number of points in road
	static const GLint VIEWSHED_MAX_POINTS = 50;

private:
	// MEMBER VARIABLES
	GLuint vao;
	const GLuint SHADOW_WIDTH = 800;
	const GLuint SHADOW_HEIGHT = 800;
	GLuint depthMap, depthMapFBO, depthMap3DTexture;

	GLfloat targetHeight;

	// For the orthogonal projection
	const glm::mat4 orthoProjMatrix = glm::ortho(-512.0f, 512.0f, -400.0f, 400.0f, 0.0f, 942.0f);
	const glm::mat4 orthoLightView = glm::lookAt(glm::vec3(-430.0f, 256.0f, 256.0f), glm::vec3(256.0f, 0.0f, 256.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 orthoLightSpaceMatrix = orthoProjMatrix * orthoLightView;

	// MEMBER OBJECTS
	Shader shader, simpleShader; // Special shader for getting depth map etc
	Terrain* terrain;
	ObsList obsList;
	DrawableModel *terrainModel, *simpleModel;

	// MEMBER METHODS
	void setupModel();
	void setupFBO();
	void setup3DDepthmapTexture();

	void renderOrtho(); // Make this private, because only getDepthMap is needed from outside
	void renderSpherical(glm::mat4, Camera*); // ditto, also needs proj and cam matrix for rendering the model
	void doRenderBoilerplate(); // Some generic code that must be done either way
	void doPostRenderBoilerplate(); // ditto but post-render
};

