#pragma once
#include "Shader.h"
#include "Terrain.h"
#include "KeyboardHandler.h"
#include "Camera.h"
#include "ViewshedProgressKeeper.h"
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
	VecList getCompletedObsPosArr();

	GLuint& getDepthMapOrtho();
	GLuint& getDepthMapSpherical(glm::mat4, Camera*);
	GLuint& get3DDepthMap();

	const glm::mat4 getOrthoLightSpaceMatrix();

	void tick(KeyboardHandler*, GLfloat elapsedFrameTime);

	GLfloat getTargetHeight() const;

	void setObserverList(ObsList);

	void renderObservers(glm::mat4&, glm::mat4&);

	void startCalc(ObsList);

	// Maximum "view" distance for the viewshed
	static const GLint VIEWSHED_MAX_DIST = 256;
	// Maximum number of points in road
	static const GLint VIEWSHED_MAX_POINTS = 50;

private:
	// MEMBER VARIABLES
	GLuint vao;
	const GLuint SHADOW_WIDTH = 2048;
	const GLuint SHADOW_HEIGHT = 2048;
	GLuint depthMap, depthMapFBO, depthMap3DTexture, depthMap2DArr;

	GLfloat targetHeight;

	// For the orthogonal projection
	const glm::mat4 orthoProjMatrix = glm::ortho(-512.0f, 512.0f, -400.0f, 400.0f, 0.0f, 942.0f);
	const glm::mat4 orthoLightView = glm::lookAt(glm::vec3(-430.0f, 256.0f, 256.0f), glm::vec3(256.0f, 0.0f, 256.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 orthoLightSpaceMatrix = orthoProjMatrix * orthoLightView;

	// MEMBER OBJECTS
	Shader shader, simpleShader; // Special shader for getting depth map etc
	Terrain* terrain;
	ObsList obsList;
	ObsList completedObservers;	// Observers that have had their viewshed calculated
	DrawableModel *terrainModel, *simpleModel;
	ViewshedProgressKeeper progressKeeper;

	// MEMBER METHODS
	void setupModel();
	void setupFBO();
	void setup3DDepthmapTexture();

	void renderOrtho(); // Make this private, because only getDepthMap is needed from outside
	void renderSpherical(GLint num); // ditto
	void doRenderBoilerplate(); // Some generic code that must be done either way
	void doPostRenderBoilerplate(); // ditto but post-render
};

