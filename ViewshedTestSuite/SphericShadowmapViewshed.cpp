#include "SphericShadowmapViewshed.h"
#include "Game.h"
#include <GL\glew.h>
#include <GL\freeglut.h>


SphericShadowmapViewshed::SphericShadowmapViewshed()
{	
	this->targetHeight = 2.0;
}

VecList SphericShadowmapViewshed::getPos() {
	VecList output;
	for (Observer &obs : obsList) {
		output.push_back(obs.getPos());
	}
	return output;
}

VecList SphericShadowmapViewshed::getCompletedObsPosArr() {
	VecList output;
	for (Observer &obs : completedObservers) {
		output.push_back(obs.getPos());
	}
	return output;
}

GLfloat SphericShadowmapViewshed::getTargetHeight() const {
	return this->targetHeight;
}

SphericShadowmapViewshed::~SphericShadowmapViewshed() {
	glDeleteVertexArrays(1, &vao);
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
}

void SphericShadowmapViewshed::initOrtho(Terrain* terrain) {
	shader = Shader("shadowmapOrtho.vert", "shadowmapOrtho.frag");
	this->terrain = terrain;
	setupModel();
	setupFBO();
}

void SphericShadowmapViewshed::initSpherical(Terrain* terrain, DrawableModel *simpleModel, Shader &simpleShader) {
	shader = Shader("shadowmapSpherical.vert", "shadowmapOrtho.frag", "shadowmapSpherical.geom");
	this->simpleModel = simpleModel;
	this->simpleShader = simpleShader;
	this->terrain = terrain;
	setupModel();
	setupFBO();
	setup3DDepthmapTexture();
}

void SphericShadowmapViewshed::setObserverList(ObsList obsListIn) {
	this->obsList = obsListIn;
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
	renderSpherical(1);
	// Then return the now-updated depthMap
	return depthMap;
}

void SphericShadowmapViewshed::startCalc(ObsList obsList) {
	this->obsList = obsList;

	// Check if we're currently doing any calculations
	if (progressKeeper.isDone()) {
		// Clear the FBO depth buffer to get ready
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear the old completed one
		completedObservers.swap(ObsList());

		progressKeeper.start(this->obsList.size());
	}
	// If we're busy, do nothing
}

GLuint& SphericShadowmapViewshed::get3DDepthMap() {
	//renderSpherical(1);
	return depthMap2DArr;
}

void SphericShadowmapViewshed::renderObservers(glm::mat4& projMatrix, glm::mat4& camMatrix) {
	for (Observer &obs : obsList) {
		obs.render(projMatrix, camMatrix);
	}

	for (Observer &obs : completedObservers) {
		obs.render(projMatrix, camMatrix);
	}
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

void SphericShadowmapViewshed::renderSpherical(GLint num) {
	if (!this->obsList.empty()) {
		doRenderBoilerplate();
		//terrainModel->prepare();
		shader.activate();

		for (int i = 0; i < num; i++) {
			// Get the first observer from the non-completed list (let's hope this copy ctor works)
			Observer obs = obsList.front();
			
			terrainModel->prepare();

			shader.uploadVec(obs.getPos(), "lightPos");
			// The slice corresponds to the elements already completed
			shader.uploadInt(completedObservers.size() * 2, "slice");
			shader.uploadFloat((GLfloat)VIEWSHED_MAX_DIST, "maxDist");
			shader.uploadMatrix(glm::mat4(1.0f), "modelMatrix");
			terrainModel->render();

			// This one has been rendered, add to completed observers and remove from the non-completed
			obs.setColor(glm::vec3(0.0, 1.0, 0.0));
			completedObservers.push_back(obs);
			obsList.erase(obsList.begin());
		}

		// Bind the 3D texture that we imageStore to
		//glBindImageTexture(0, depthMap3DTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R32F);
		/*GLint counter = 0;
		for (Observer &obs : obsList) {
			terrainModel->prepare();
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shader.uploadVec(obs.getPos(), "lightPos");
			shader.uploadInt(counter, "slice");
			shader.uploadFloat((GLfloat)VIEWSHED_MAX_DIST, "maxDist");
			shader.uploadMatrix(glm::mat4(1.0f), "modelMatrix");
			terrainModel->render();
			counter++;
			//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}*/
		// To make sure that changes are written before sampling the 3D texture elsewhere
		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		shader.deactivate();
		doPostRenderBoilerplate();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}	
}

void SphericShadowmapViewshed::doRenderBoilerplate() {
	// Do some boilerplate
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Now we can do the render
	//glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
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
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void SphericShadowmapViewshed::setupFBO() {
	// Generate framebuffer object
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	// Generate depth map
	/*glGenTextures(1, &depthMap);
	
	// Setup the depth map texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/

	// Setup of 2D tex array
	glGenTextures(1, &depthMap2DArr);

	// Parameters
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap2DArr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, VIEWSHED_MAX_POINTS, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	// Attach depth map texture to the FBO
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap2DArr, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
	}

	// Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SphericShadowmapViewshed::setup3DDepthmapTexture() {
	glGenTextures(1, &depthMap3DTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, depthMap3DTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, SHADOW_WIDTH, SHADOW_HEIGHT, VIEWSHED_MAX_POINTS, 0, GL_RED, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_3D, 0);
}

void SphericShadowmapViewshed::setupModel() {
	terrainModel = terrain->getTerrainModel();
	terrainModel->addShader(shader);
}

void SphericShadowmapViewshed::tick(KeyboardHandler* handler, GLfloat elapsedFrameTime) {
	// Update the observers
	for (Observer &obs : obsList) {
		obs.tick(handler);
	}

	if (handler->keyStates['m']) {
		this->targetHeight += 0.2;
		printf("Target height is now %f\n", this->targetHeight);
	}		
	else if (handler->keyStates['n']) {
		this->targetHeight -= 0.2;
		printf("Target height is now %f\n", this->targetHeight);
	}		

	// Do necessary draw calls
	if (!progressKeeper.isDone()) {
		GLint num = progressKeeper.getNumberOfCallsForFrame(elapsedFrameTime);
		printf("Doing %d draw calls this frame\n", num);
		GLfloat frameStart = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
		renderSpherical(num);
		glFinish();
		GLfloat elapsedFrameTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME) - frameStart;
		printf("elapsedTime: %.6f \n", elapsedFrameTime);
	}
}