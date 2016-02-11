#include "SphericShadowmapViewshed.h"
#include "Game.h"
#include <GL\glew.h>
#include <GL\freeglut.h>


SphericShadowmapViewshed::SphericShadowmapViewshed()
{	
	this->pos = glm::vec3(0, 0, 0);
}

void SphericShadowmapViewshed::setPos(glm::vec3 newPos) {
	this->pos = newPos;
}

SphericShadowmapViewshed::~SphericShadowmapViewshed() {
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
}

void SphericShadowmapViewshed::init(Terrain* terrain) {
	shader = Shader("shadowmap.vert", "shadowmap.frag");
	this->terrain = terrain;
	setupVAO();
	setupFBO();
}

const glm::mat4 SphericShadowmapViewshed::getLightSpaceMatrix() {
	return lightSpaceMatrix;
}

GLuint& SphericShadowmapViewshed::getDepthMap() {
	// First render the scene
	render();
	// Then return the depthmap
	return depthMap;
}

void SphericShadowmapViewshed::render() {
	// Start with setting the viewport (since shadow map is not necessarily same resolution as window)
	// After that, bind FBO
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	// Now we can do the render
	glClear(GL_DEPTH_BUFFER_BIT);
	shader.activate();
	glBindVertexArray(vao);
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	shader.uploadMatrix(lightSpaceMatrix, "lightSpaceMatrix");
	glDrawElements(GL_TRIANGLES, terrain->getTriangleCount() * 3, GL_UNSIGNED_INT, 0L);

	shader.deactivate();
	glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset the viewport out of courtesy
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