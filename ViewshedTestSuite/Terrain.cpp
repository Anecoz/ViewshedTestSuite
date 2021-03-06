#include "Terrain.h"
#include "FileHandler.h"
#include "SphericShadowmapViewshed.h"
#include "Game.h"
#include "Voxelizer.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

extern "C" {
#include "GL_utilities.h" 
}

using namespace std;

Terrain::Terrain()
{
	
}

Terrain::~Terrain()
{
	// The model objects will free their respective VBOs, VAOs and arrays
	delete minimapModel;
	delete terrainModel;
	minimapModel = nullptr;
	terrainModel = nullptr;
}

DrawableModel *Terrain::getTerrainModel() {
	return terrainModel;
}

const GLuint Terrain::getTriangleCount() {
	return TRIANGLE_COUNT;
}

VoxelContainer& Terrain::getVoxels() {
	return voxels;
}

void Terrain::init(DrawableModel *simpleModel) {
	orthoShader = Shader("terrainOrtho.vert", "terrainOrtho.frag");
	sphericalShader = Shader("terrainSpherical.vert", "terrainSpherical.frag");
	modelShader = Shader("terrainShadowmap.vert", "terrainShadowmap.frag");
	voxelShader = Shader("terrainVoxel.vert", "terrainVoxel.frag");
	this->encodePosShader = Shader("encodePosition.vert", "encodePosition.frag");

	this->simpleModel = simpleModel;
	this->simpleModel->addShader(encodePosShader);

	setupModels();
	setupFBO();
}

GLuint& Terrain::getEncodedPosTex(glm::mat4& camMatrix, glm::mat4& projMatrix, RoadSelector* roadSelector) {
	// Render to texture first, then return the texture handle
	renderPositionEncoding(camMatrix, projMatrix, roadSelector);

	return posTex;
}

void Terrain::renderPositionEncoding(glm::mat4& camMatrix, glm::mat4& projMatrix, RoadSelector* roadSelector) {
	glBindFramebuffer(GL_FRAMEBUFFER, posFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Only clears this FBO
	doRenderBoilerplate();
	terrainModel->prepare();
	encodePosShader.activate();

	encodePosShader.uploadMatrix(projMatrix, "projMatrix");
	encodePosShader.uploadMatrix(camMatrix, "camMatrix");	
	encodePosShader.uploadMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)), "modelMatrix");

	// Do the draw call, will render to the texture posTex
	terrainModel->render();

	// Loop over all the road points, to get them encoded aswell
	PointList pointList = roadSelector->getPointList();

	/*glDisable(GL_CULL_FACE);
	for (glm::vec3 &point : pointList) {
		simpleModel->prepare();

		encodePosShader.uploadMatrix(camMatrix, "camMatrix");
		encodePosShader.uploadMatrix(projMatrix, "projMatrix");
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), point);
		encodePosShader.uploadMatrix(translationMatrix, "modelMatrix");

		simpleModel->render();
	}*/

	doPostRenderBoilerplate();
	encodePosShader.deactivate();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Terrain::renderOrtho(glm::mat4 camMatrix, glm::mat4 projMatrix, glm::mat4 lightSpaceMatrix, GLuint& depthMap) {
	orthoShader.activate();
	terrainModel->prepare();
	doRenderBoilerplate();

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	orthoShader.uploadTexture(0, "depthMap");

	// matrix uploads
	orthoShader.uploadMatrix(camMatrix, "camMatrix");
	orthoShader.uploadMatrix(projMatrix, "projMatrix");
	orthoShader.uploadMatrix(lightSpaceMatrix, "lightSpaceMatrix");
	
	terrainModel->render();

	orthoShader.deactivate();
	doPostRenderBoilerplate();
}

void Terrain::renderVoxelized(glm::mat4 camMatrix, glm::mat4 projMatrix, GLuint& voxTex, VecList lightArr, GLfloat targetHeight, GLint voxTexDim) {
	doRenderBoilerplate();
	terrainModel->prepare();
	voxelShader.activate();

	// Bind and upload 3d texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxTex);
	voxelShader.uploadTexture(0, "voxTex");	

	// Matrix uploads
	voxelShader.uploadFloat(targetHeight, "targetHeight");
	voxelShader.uploadMatrix(camMatrix, "camMatrix");
	voxelShader.uploadMatrix(projMatrix, "projMatrix");
	if (!lightArr.empty())
		voxelShader.uploadVecArr(lightArr, "lightArr");
	voxelShader.uploadFloat((GLfloat)SphericShadowmapViewshed::VIEWSHED_MAX_DIST, "maxDist");
	voxelShader.uploadInt(lightArr.size(), "numObs");
	voxelShader.uploadInt(Voxelizer::WIDTH, "voxelDim");
	voxelShader.uploadInt(voxTexDim, "voxTexDim");
	voxelShader.uploadVec(glm::vec3(Voxelizer::WIDTH / 2.0, Voxelizer::HEIGHT / 2.0, Voxelizer::DEPTH / 2.0), "topNodePos");
	voxelShader.uploadInt(Voxelizer::WIDTH, "topNodeSize");
	//printf("numObs: %d\n", lightArr.size());

	// Draw
	terrainModel->render();

	voxelShader.deactivate();

	// DEBUG
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, posTex);
	GLfloat *pixels;
	pixels = (GLfloat*) malloc(sizeof(GLfloat) * Game::WINDOW_SIZE_X * Game::WINDOW_SIZE_Y*3);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pixels);*/

	/*GLfloat currMax = 0.0;
	for (int x = 0; x < Game::WINDOW_SIZE_X*Game::WINDOW_SIZE_Y*3; x++) {
		printf("texture: %f, currMax is: %f\n", pixels[x], currMax);
		if (pixels[x] > currMax)
			currMax = pixels[x];
	}*/

	// After this, render only the color encoded map to top right of the screen
	/*GLint wpos = Game::WINDOW_SIZE_X - Game::WINDOW_SIZE_X / 3.0;
	GLint hpos = Game::WINDOW_SIZE_Y - Game::WINDOW_SIZE_Y / 3.0;
 	glViewport(wpos, hpos, Game::WINDOW_SIZE_X / 3.0, Game::WINDOW_SIZE_Y / 3.0);
	minimapModel->prepare();
	modelShader.activate();

	// Upload texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, posTex);
	modelShader.uploadBool(true, "isColor");
	modelShader.uploadTexture(0, "depthMap");
	
	minimapModel->render();

	modelShader.deactivate();
	// Reset viewport
	glViewport(0, 0, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
	doPostRenderBoilerplate();*/
}

void Terrain::renderSpherical(glm::mat4 camMatrix, glm::mat4 projMatrix, GLuint& depthMap3DTexture, VecList lightArr, GLfloat targetHeight) {
	sphericalShader.activate();
	terrainModel->prepare();
	doRenderBoilerplate();

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_3D, depthMap3DTexture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depthMap3DTexture);
	sphericalShader.uploadTexture(0, "depthMap");

	// Matrix uploads
	sphericalShader.uploadMatrix(camMatrix, "camMatrix");
	sphericalShader.uploadMatrix(projMatrix, "projMatrix");
	sphericalShader.uploadInt(lightArr.size(), "numObs");
	if (!lightArr.empty())
		sphericalShader.uploadVecArr(lightArr, "lightArr");
		//sphericalShader.uploadVec(lightArr.at(0), "lightPos");
	
	sphericalShader.uploadFloat((GLfloat)SphericShadowmapViewshed::VIEWSHED_MAX_DIST, "maxDist");
	sphericalShader.uploadFloat(targetHeight, "targetHeight");
	
	terrainModel->render();

	sphericalShader.deactivate();
	// Clear the texture
	int level = 0;
	//for (glm::vec3 &point : lightArr) {
		//glClearTexImage(depthMap3DTexture, level, GL_RED, GL_FLOAT, 0);
		//level++;
	//}
	//glBindTexture(GL_TEXTURE_2D, 0);
	doPostRenderBoilerplate();

	// After this, render only the depth map to top right of the screen
	GLint wpos = Game::WINDOW_SIZE_X - Game::WINDOW_SIZE_X / 3.0;
	GLint hpos = Game::WINDOW_SIZE_Y - Game::WINDOW_SIZE_Y / 3.0;
	glViewport(wpos, hpos, Game::WINDOW_SIZE_X / 3.0, Game::WINDOW_SIZE_Y / 3.0);
	minimapModel->prepare();
	modelShader.activate();

	// Upload texture
	modelShader.uploadBool(false, "isColor");
	modelShader.uploadTexture(0, "depthMap");
	
	minimapModel->render();

	glBindVertexArray(0);
	modelShader.deactivate();
	// Reset viewport
	glViewport(0, 0, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
}

void Terrain::doRenderBoilerplate() {
	//glBindVertexArray(vao);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);	
}

void Terrain::doPostRenderBoilerplate() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void Terrain::setupFBO() {
	// Generate framebuffer object
	glGenFramebuffers(1, &posFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, posFBO);

	// Generate depth map
	glGenTextures(1, &posTex);

	// Setup the position encoding texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, posTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Attach depth map texture to the FBO
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	
	// Bind the color texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);

	// Error checking
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
	}

	// Unbind FBO and texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Terrain::setupModels() {
	// Start with filling the arrays

	GLfloat *modelVertexArray = (GLfloat*)malloc(4 * 3 * sizeof(GLfloat));
	GLuint *modelIndexArray = (GLuint*)malloc(2 * 3 * sizeof(GLuint));

	// 4 vertices
	modelVertexArray[0] = -1.0;
	modelVertexArray[1] = 1.0;
	modelVertexArray[2] = 0.0;

	modelVertexArray[3] = -1.0;
	modelVertexArray[4] = -1.0;
	modelVertexArray[5] = 0.0;

	modelVertexArray[6] = 1.0;
	modelVertexArray[7] = 1.0;
	modelVertexArray[8] = 0.0;

	modelVertexArray[9] = 1.0;
	modelVertexArray[10] = -1.0;
	modelVertexArray[11] = 0.0;

	// Indices, triangle 1
	modelIndexArray[0] = 0;
	modelIndexArray[1] = 1;
	modelIndexArray[2] = 2;

	// Triangle 2
	modelIndexArray[3] = 2;
	modelIndexArray[4] = 1;
	modelIndexArray[5] = 3;

	minimapModel = new DrawableModel(modelVertexArray, nullptr, modelIndexArray);
	ShaderList shaders;
	shaders.push_back(modelShader);
	minimapModel->init(shaders, 4, 2);

	// Now do the terrain model
	GLfloat *vertexArr = nullptr, *normalArr = nullptr;
	GLuint *indexArr = nullptr;
	indexArr = (GLuint *)malloc(sizeof(GLuint) * TRIANGLE_COUNT * 3);
	vertexArr = (GLfloat *)malloc(sizeof(GLfloat) * VERTEX_COUNT * 3);
	normalArr = (GLfloat *)malloc(sizeof(GLfloat) * VERTEX_COUNT * 3);
	generator.generate(&voxels, vertexArr, normalArr, indexArr, TILE_SIZE, TRIANGLE_COUNT, VERTEX_COUNT);
	
	terrainModel = new DrawableModel(vertexArr, normalArr, indexArr);
	ShaderList terrainShaders;
	terrainShaders.push_back(orthoShader);
	terrainShaders.push_back(sphericalShader);
	terrainShaders.push_back(voxelShader);
	terrainModel->init(terrainShaders, VERTEX_COUNT, TRIANGLE_COUNT);
}
