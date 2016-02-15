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
	glDeleteVertexArrays(1, &modelVAO);
	glDeleteBuffers(1, &modelVertexVBO);
	glDeleteBuffers(1, &modelIndexVBO);
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);

	free(vertexArray);
	free(indexArray);
}

void SphericShadowmapViewshed::initOrtho(Terrain* terrain) {
	shader = Shader("shadowmapOrtho.vert", "shadowmapOrtho.frag");
	modelShader.id = -1;
	this->terrain = terrain;
	setupVAO();
	setupFBO();
}

void SphericShadowmapViewshed::initSpherical(Terrain* terrain) {
	shader = Shader("shadowmapSpherical.vert", "shadowmapOrtho.frag");
	modelShader = Shader("shadowModel.vert", "shadowModel.frag");
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

	shader.uploadVec(this->pos, "lightPos");
	shader.uploadFloat((GLfloat)VIEWSHED_MAX_DIST, "maxDist");
	shader.uploadMatrix(glm::mat4(1.0f), "modelMatrix");
	glDrawElements(GL_TRIANGLES, terrain->getTriangleCount() * 3, GL_UNSIGNED_INT, 0L);
	// Render the camera as a quad
	glBindVertexArray(modelVAO);
	shader.uploadMatrix(glm::translate(glm::mat4(1.0f), camera->getPos()), "modelMatrix");
	glDrawElements(GL_TRIANGLES, MODEL_TRIANGLE_COUNT * 3, GL_UNSIGNED_INT, 0L);

	shader.deactivate();
	glBindVertexArray(0);
	doPostRenderBoilerplate();

	// Also render a representation of the viewshed object
	glBindVertexArray(modelVAO);
	glEnable(GL_DEPTH_TEST);
	modelShader.activate();

	modelShader.uploadMatrix(projMatrix, "projMatrix");
	modelShader.uploadMatrix(camera->getCameraMatrix(), "camMatrix");
	// Transformation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), this->pos);
	modelShader.uploadMatrix(translationMatrix, "modelMatrix");
	glDrawElements(GL_TRIANGLES, MODEL_TRIANGLE_COUNT * 3, GL_UNSIGNED_INT, 0L);

	glBindVertexArray(0);
	glDisable(GL_DEPTH_TEST);
	modelShader.deactivate();
	
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
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
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

	// Also init our model and its vao
	if (modelShader.id != -1) {
		setupModel();
	}
}

void SphericShadowmapViewshed::setupModel() {
	// Start with filling the arrays

	vertexArray = (GLfloat*)malloc(MODEL_VERTEX_COUNT * 3 * sizeof(GLfloat));
	indexArray = (GLuint*)malloc(MODEL_TRIANGLE_COUNT * 3 * sizeof(GLuint));

	// 4 vertices
	vertexArray[0] = -1.0;
	vertexArray[1] = 1.0;
	vertexArray[2] = 0.0;

	vertexArray[3] = -1.0;
	vertexArray[4] = -1.0;
	vertexArray[5] = 0.0;

	vertexArray[6] = 1.0;
	vertexArray[7] = 1.0;
	vertexArray[8] = 0.0;

	vertexArray[9] = 1.0;
	vertexArray[10] = -1.0;
	vertexArray[11] = 0.0;

	// Indices, triangle 1
	indexArray[0] = 0;
	indexArray[1] = 1;
	indexArray[2] = 2;

	// Triangle 2
	indexArray[3] = 2;
	indexArray[4] = 1;
	indexArray[5] = 3;

	// Fill up the VAO
	modelShader.activate();
	glGenVertexArrays(1, &modelVAO);
	glBindVertexArray(modelVAO);
	glGenBuffers(1, &modelVertexVBO);
	glGenBuffers(1, &modelIndexVBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, modelVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, MODEL_VERTEX_COUNT * 3 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);
	modelShader.setAndEnableVertexAttrib("inPosition");
	modelShader.deactivate();
	shader.activate();
	shader.setAndEnableVertexAttrib("inPosition");
	shader.deactivate();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MODEL_TRIANGLE_COUNT * 3 * sizeof(GLuint), indexArray, GL_STATIC_DRAW);
	
	glBindVertexArray(0);
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