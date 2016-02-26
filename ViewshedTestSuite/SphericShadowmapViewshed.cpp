#include "SphericShadowmapViewshed.h"
#include "Game.h"
#include <GL\glew.h>
#include <GL\freeglut.h>


SphericShadowmapViewshed::SphericShadowmapViewshed()
{	
	this->observer.setPos(glm::vec3(256, 20, 256));
	this->targetHeight = 0.0;
}

glm::vec3 SphericShadowmapViewshed::getPos() {
	return observer.getPos();
}

GLfloat SphericShadowmapViewshed::getTargetHeight() const {
	return this->targetHeight;
}

SphericShadowmapViewshed::~SphericShadowmapViewshed() {
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);

	free(vertexArray);
	free(indexArray);
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
	observer.init();
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

GLuint& SphericShadowmapViewshed::getDepthMapSpherical(glm::mat4 projMatrix, Camera* camera) {
	// render scene using the spherical pipeline
	renderSpherical(projMatrix, camera);
	// Then return the now-updated depthMap
	return depthMap;
}

void SphericShadowmapViewshed::renderOrtho() {
	doRenderBoilerplate();
	glBindVertexArray(vao);
	shader.activate();

	shader.uploadMatrix(orthoLightSpaceMatrix, "lightSpaceMatrix");
	glDrawElements(GL_TRIANGLES, terrain->getTriangleCount() * 3, GL_UNSIGNED_INT, 0L);

	shader.deactivate();
	glBindVertexArray(0);
	doPostRenderBoilerplate();
}

void SphericShadowmapViewshed::renderSpherical(glm::mat4 projMatrix, Camera* camera) {
	doRenderBoilerplate();
	glBindVertexArray(vao);
	shader.activate();

	shader.uploadVec(this->observer.getPos(), "lightPos");
	shader.uploadFloat((GLfloat)VIEWSHED_MAX_DIST, "maxDist");
	shader.uploadMatrix(glm::mat4(1.0f), "modelMatrix");
	glDrawElements(GL_TRIANGLES, terrain->getTriangleCount() * 3, GL_UNSIGNED_INT, 0L);

	shader.deactivate();
	glBindVertexArray(0);
	doPostRenderBoilerplate();

	// Also render observer
	observer.render(projMatrix, camera);
}

void SphericShadowmapViewshed::doRenderBoilerplate() {
	// Do some boilerplate
	// Start with setting the viewport (since shadow map is not necessarily same resolution as window)
	// After that, bind FBO
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	// Now we can do the render
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void SphericShadowmapViewshed::doPostRenderBoilerplate() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
	// Update the observer
	observer.tick(handler);

	if (handler->keyStates['m'])
		this->targetHeight += 0.2;
	else if (handler->keyStates['n'])
		this->targetHeight -= 0.2;

	//printf("Pos is now %f %f %f\n", this->pos.x, this->pos.y, this->pos.z);
	//printf("Observer height: %f\n", this->observerHeight);
}