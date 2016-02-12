#include "SphericShadowmapViewshed.h"
#include "Game.h"
#include <GL\glew.h>
#include <GL\freeglut.h>


SphericShadowmapViewshed::SphericShadowmapViewshed()
{	
	this->pos = glm::vec3(256, 20, 256);
}

void SphericShadowmapViewshed::setPos(glm::vec3 newPos) {
	this->pos = newPos;
}

glm::vec3 SphericShadowmapViewshed::getPos() {
	return this->pos;
}

SphericShadowmapViewshed::~SphericShadowmapViewshed() {
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
}

void SphericShadowmapViewshed::initOrtho(Terrain* terrain) {
	shader = Shader("shadowmapOrtho.vert", "shadowmapOrtho.frag");
	this->terrain = terrain;
	setupVAO();
	setupFBO();
}

void SphericShadowmapViewshed::initSpherical(Terrain* terrain) {
	shader = Shader("shadowmapSpherical.vert", "shadowmapOrtho.frag");
	this->terrain = terrain;
	setupVAO();
	setupFBO();
}

const glm::mat4 SphericShadowmapViewshed::getOrthoLightSpaceMatrix() {
	return orthoLightSpaceMatrix;
}

GLuint& SphericShadowmapViewshed::getDepthMapOrtho() {
	// First render the scene
	renderOrtho();
	// Then return the depthmap
	return depthMap;
}

GLuint& SphericShadowmapViewshed::getDepthMapSpherical() {
	// render scene using the spherical pipeline
	renderSpherical();
	// Then return the now-updated depthMap
	return depthMap;
}

void SphericShadowmapViewshed::renderOrtho() {
	doRenderBoilerplate();

	shader.uploadMatrix(orthoLightSpaceMatrix, "lightSpaceMatrix");
	glDrawElements(GL_TRIANGLES, terrain->getTriangleCount() * 3, GL_UNSIGNED_INT, 0L);

	doPostRenderBoilerplate();
}

void SphericShadowmapViewshed::renderSpherical() {
	doRenderBoilerplate();

	shader.uploadVec(this->pos, "lightPos");
	glDrawElements(GL_TRIANGLES, terrain->getTriangleCount() * 3, GL_UNSIGNED_INT, 0L);

	doPostRenderBoilerplate();
}

void SphericShadowmapViewshed::doRenderBoilerplate() {
	// Do some boilerplate
	// Start with setting the viewport (since shadow map is not necessarily same resolution as window)
	// After that, bind FBO
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	// Now we can do the render
	glClear(GL_DEPTH_BUFFER_BIT);
	shader.activate();
	glBindVertexArray(vao);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void SphericShadowmapViewshed::doPostRenderBoilerplate() {
	shader.deactivate();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset the viewport out of courtesy. 
	// And also because it will otherwise lead to ridiculously strange things outside of this class
	glViewport(0, 0, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
}

void SphericShadowmapViewshed::setupFBO() {
	// Generate framebuffer object
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	// Generate depth map
	glGenTextures(1, &depthMap);
	
	// Setup the depth map texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach depth map texture to the FBO	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
	}

	// Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SphericShadowmapViewshed::setupVAO() {
	shader.activate();
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Get the VBOs from the terrain
	glBindBuffer(GL_ARRAY_BUFFER, *terrain->getVertexVBO());
	shader.setAndEnableVertexAttrib("inPosition");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *terrain->getIndexVBO());

	glBindVertexArray(0);
	shader.deactivate();
}

void SphericShadowmapViewshed::tick(KeyboardHandler* handler) {
	// Update the position of the light based on input
	GLfloat velocity = 1.0;

	if (handler->keyStates['i'])
		this->pos.x += velocity;
	else if (handler->keyStates['k'])
		this->pos.x -= velocity;
	else if (handler->keyStates['j'])
		this->pos.z += velocity;
	else if (handler->keyStates['l'])
		this->pos.z -= velocity;
	else if (handler->keyStates['o'])
		this->pos.y += velocity;
	else if (handler->keyStates['u'])
		this->pos.y -= velocity;
}