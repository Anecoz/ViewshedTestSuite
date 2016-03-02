#include "Terrain.h"
#include "FileHandler.h"
#include "SphericShadowmapViewshed.h"
#include "Game.h"
#include <vector>

using namespace std;


Terrain::Terrain()
{
	
}

Terrain::~Terrain()
{
	// Take care of all VBOs in here, freeing them
	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &modelVAO);

	glDeleteBuffers(1, &modelVertexVBO);
	glDeleteBuffers(1, &modelIndexVBO);

	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &indexVBO);
	glDeleteBuffers(1, &normalVBO);

	free(vertexArray);
	free(indexArray);
	free(normalArray);
	free(modelVertexArray);
	free(modelIndexArray);
}

GLuint* Terrain::getVertexVBO() {
	return &vertexVBO;
}

GLuint* Terrain::getIndexVBO() {
	return &indexVBO;
}

const GLuint Terrain::getTriangleCount() {
	return TRIANGLE_COUNT;
}

VoxelContainer& Terrain::getVoxels() {
	return voxels;
}

void Terrain::init() {
	orthoShader = Shader("terrainOrtho.vert", "terrainOrtho.frag");
	sphericalShader = Shader("terrainSpherical.vert", "terrainSpherical.frag");
	modelShader = Shader("terrainShadowmap.vert", "terrainShadowmap.frag");
	voxelShader = Shader("terrainVoxel.vert", "terrainVoxel.frag");
	encodePosShader = Shader("encodePosition.vert", "encodePosition.frag");
}

GLuint& Terrain::getEncodedPosTex(glm::mat4& camMatrix, glm::mat4& projMatrix) {
	// Render to texture first, then return the texture handle
	renderPositionEncoding(camMatrix, projMatrix);

	return posTex;
}

void Terrain::renderPositionEncoding(glm::mat4& camMatrix, glm::mat4& projMatrix) {
	glBindFramebuffer(GL_FRAMEBUFFER, posFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Only clears this FBO
	doRenderBoilerplate();
	encodePosShader.activate();	

	encodePosShader.uploadMatrix(camMatrix, "camMatrix");
	encodePosShader.uploadMatrix(projMatrix, "projMatrix");

	// Do the draw call, will render to the texture posTex
	glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * 3, GL_UNSIGNED_INT, 0L);

	doPostRenderBoilerplate();
	encodePosShader.deactivate();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Terrain::renderOrtho(glm::mat4 camMatrix, glm::mat4 projMatrix, glm::mat4 lightSpaceMatrix, GLuint& depthMap) {
	orthoShader.activate();
	doRenderBoilerplate();

	// Bind texture
	activateDepthTexture();
	glBindTexture(GL_TEXTURE_2D, depthMap);
	orthoShader.uploadTexture(0, "depthMap");

	// matrix uploads
	orthoShader.uploadMatrix(camMatrix, "camMatrix");
	orthoShader.uploadMatrix(projMatrix, "projMatrix");
	orthoShader.uploadMatrix(lightSpaceMatrix, "lightSpaceMatrix");
	glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT*3, GL_UNSIGNED_INT, 0L);

	orthoShader.deactivate();
	doPostRenderBoilerplate();
}

void Terrain::renderVoxelized(glm::mat4 camMatrix, glm::mat4 projMatrix, GLuint& voxTex, VecList lightArr) {
	doRenderBoilerplate();
	voxelShader.activate();

	// Bind and upload 3d texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxTex);
	voxelShader.uploadTexture(0, "voxTex");	

	// Matrix uploads
	voxelShader.uploadMatrix(camMatrix, "camMatrix");
	voxelShader.uploadMatrix(projMatrix, "projMatrix");
	voxelShader.uploadVecArr(lightArr, "lightArr");
	voxelShader.uploadFloat((GLfloat)SphericShadowmapViewshed::VIEWSHED_MAX_DIST, "maxDist");

	// Draw
	glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * 3, GL_UNSIGNED_INT, 0L);

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

	// After this, render only the depth map to top right of the screen
	GLint wpos = Game::WINDOW_SIZE_X - Game::WINDOW_SIZE_X / 3.0;
	GLint hpos = Game::WINDOW_SIZE_Y - Game::WINDOW_SIZE_Y / 3.0;
 	glViewport(wpos, hpos, Game::WINDOW_SIZE_X / 3.0, Game::WINDOW_SIZE_Y / 3.0);
	modelShader.activate();
	glBindVertexArray(modelVAO);

	// Upload texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, posTex);
	modelShader.uploadBool(true, "isColor");
	modelShader.uploadTexture(0, "depthMap");
	glDrawElements(GL_TRIANGLES, MODEL_TRIANGLE_COUNT * 3, GL_UNSIGNED_INT, 0L);

	glBindVertexArray(0);
	modelShader.deactivate();
	// Reset viewport
	glViewport(0, 0, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
	doPostRenderBoilerplate();
}

void Terrain::renderSpherical(glm::mat4 camMatrix, glm::mat4 projMatrix, GLuint& depthMap, glm::vec3 lightPos, GLfloat obsHeight) {
	sphericalShader.activate();
	doRenderBoilerplate();

	// Bind texture
	activateDepthTexture();
	glBindTexture(GL_TEXTURE_2D, depthMap);
	orthoShader.uploadTexture(0, "depthMap");

	// Matrix uploads
	sphericalShader.uploadMatrix(camMatrix, "camMatrix");
	sphericalShader.uploadMatrix(projMatrix, "projMatrix");
	sphericalShader.uploadVec(lightPos, "lightPos");
	sphericalShader.uploadFloat((GLfloat)SphericShadowmapViewshed::VIEWSHED_MAX_DIST, "maxDist");
	sphericalShader.uploadFloat(obsHeight, "observerHeight");
	glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * 3, GL_UNSIGNED_INT, 0L);

	sphericalShader.deactivate();
	doPostRenderBoilerplate();

	// After this, render only the depth map to top right of the screen
	GLint wpos = Game::WINDOW_SIZE_X - Game::WINDOW_SIZE_X / 3.0;
	GLint hpos = Game::WINDOW_SIZE_Y - Game::WINDOW_SIZE_Y / 3.0;
	glViewport(wpos, hpos, Game::WINDOW_SIZE_X / 3.0, Game::WINDOW_SIZE_Y / 3.0);
	modelShader.activate();
	glBindVertexArray(modelVAO);

	// Upload texture
	modelShader.uploadBool(false, "isColor");
	modelShader.uploadTexture(0, "depthMap");
	glDrawElements(GL_TRIANGLES, MODEL_TRIANGLE_COUNT * 3, GL_UNSIGNED_INT, 0L);

	glBindVertexArray(0);
	modelShader.deactivate();
	// Reset viewport
	glViewport(0, 0, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
}

void Terrain::doRenderBoilerplate() {
	glBindVertexArray(vao);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);	
}

void Terrain::doPostRenderBoilerplate() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glBindVertexArray(0);
}

void Terrain::activateDepthTexture() {
	glActiveTexture(GL_TEXTURE0);
}

void Terrain::generate() {
	// Check first if terrain already exists on file
	/*if (FileHandler::checkIfExists(VERTEX_FILE_NAME)) {
		printf("Terrain is on file, loading...\n");

		// Load vertices, indices and normals from file
		vertexArray = FileHandler::loadFloatArrFromFile(VERTEX_FILE_NAME);
		printf("vertexArray loaded\n");
		normalArray = FileHandler::loadFloatArrFromFile(NORMAL_FILE_NAME);
		printf("normalArray loaded\n");
		indexArray = FileHandler::loadUintArrFromFile(INDEX_FILE_NAME);
		printf("indexArray loaded\n");

		if (vertexArray != nullptr && normalArray != nullptr && indexArray != nullptr)
			printf("Loaded terrain from file!\n");
		else {
			printf("Could not load terrain from file, exiting.\n");
			exit(4);
		}	
	}
	else {*/
		printf("Terrain is not on file, generating on CPU\n");

		// Start with setting some constants
		GLint width = TILE_SIZE;
		GLint height = TILE_SIZE;

		// Vectors to hold vertex data as well as indices
		indexArray = (GLuint *)malloc(sizeof(GLuint) * TRIANGLE_COUNT * 3);
		vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * VERTEX_COUNT * 3);
		normalArray = (GLfloat *)malloc(sizeof(GLfloat) * VERTEX_COUNT * 3);

		// Start with initializing voxelarray to 0's
		printf("Initializing voxelarray to 0...\n");
		voxels.init();
		printf("Initialization of voxelarray done, generating terrain...\n");

		// Now loop over both x and z and set the vertices +  normals
		for (int x = 0; x < width; x++)
			for (int z = 0; z < height; z++) {
				GLfloat height = generator.getHeight((GLfloat)x, (GLfloat)z);
				vertexArray[(x + z * width) * 3 + 0] = (GLfloat)x;
				vertexArray[(x + z * width) * 3 + 1] = height; // Height (this is 2.5D)
				vertexArray[(x + z * width) * 3 + 2] = (GLfloat)z;

				// Update the voxels
				voxels.setValue(x, floor(height), z, 1);

				glm::vec3 normal = calcNormal((GLfloat)x, height, (GLfloat)z);
				// Set this in the array
				normalArray[(x + z * width) * 3 + 0] = normal.x;
				normalArray[(x + z * width) * 3 + 1] = normal.y;
				normalArray[(x + z * width) * 3 + 2] = normal.z;
			}

		// Same for indices
		for (int x = 0; x < width - 1; x++)
			for (int z = 0; z < height - 1; z++) {
				// Triangle 1
				indexArray[(x + z * (width - 1)) * 6 + 0] = x + z * width;
				indexArray[(x + z * (width - 1)) * 6 + 1] = x + (z + 1) * width;
				indexArray[(x + z * (width - 1)) * 6 + 2] = x + 1 + z * width;
				// Triangle 2
				indexArray[(x + z * (width - 1)) * 6 + 3] = x + 1 + z * width;
				indexArray[(x + z * (width - 1)) * 6 + 4] = x + (z + 1) * width;
				indexArray[(x + z * (width - 1)) * 6 + 5] = x + 1 + (z + 1) * width;
			}

		// DEBUG
		/*
		for (int x = 0; x < 512; x++)
			for (int y = 0; y < 128; y++)
				for (int z = 0; z < 512; z++) {
					printf("voxel at %d, %d, %d is %d\n", x, y, z, voxels.getValue(x, y, z));
				}*/

		//printf("Generation done! Saving to file... \n");

		/*bool vertSaveRes = FileHandler::saveToFile(VERTEX_FILE_NAME, vertexArray, VERTEX_COUNT * 3);
		bool normSaveRes = FileHandler::saveToFile(NORMAL_FILE_NAME, normalArray, VERTEX_COUNT * 3);
		bool indSaveRes = FileHandler::saveToFile(INDEX_FILE_NAME, indexArray, TRIANGLE_COUNT * 3);

		if (vertSaveRes && normSaveRes && indSaveRes) {
			printf("Terrain saved to file.\n");
		}
		else {
			printf("Could not save terrain to file, exiting\n");
			exit(3);
		}*/
	//}

	// Now construct a VAO from this data.
	setupVAO();
	setupFBO();
}

glm::vec3 Terrain::calcNormal(GLfloat x, GLfloat y, GLfloat z) {
	GLfloat offset = 0.01;

	glm::vec3 normal = { 0.0, y, 0.0 };
	glm::vec3 v1 = normal - glm::vec3(offset, generator.getHeight(x + offset, z), 0.0);
	glm::vec3 v2 = normal - glm::vec3(0.0, generator.getHeight(x, z - offset), -offset);
	normal = glm::cross(v1, v2);
	normal = glm::normalize(normal);

	return normal;
}

void Terrain::setupVAO() {
	// Generate a new VAO as well as VBOs, and construct it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &normalVBO);
	glGenBuffers(1, &indexVBO);

	// Vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * 3 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);

	orthoShader.activate();
	orthoShader.setAndEnableVertexAttrib(VERTEX_IN_NAME);
	orthoShader.deactivate();
	
	sphericalShader.activate();
	sphericalShader.setAndEnableVertexAttrib(VERTEX_IN_NAME);
	sphericalShader.deactivate();

	voxelShader.activate();
	voxelShader.setAndEnableVertexAttrib(VERTEX_IN_NAME);
	voxelShader.deactivate();

	// Normal VBO
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, VERTEX_COUNT * 3 * sizeof(GLfloat), normalArray, GL_STATIC_DRAW);
	
	orthoShader.activate();
	orthoShader.setAndEnableNormalAttrib(NORMAL_IN_NAME);
	orthoShader.deactivate();

	sphericalShader.activate();
	sphericalShader.setAndEnableNormalAttrib(NORMAL_IN_NAME);
	sphericalShader.deactivate();

	voxelShader.activate();
	voxelShader.setAndEnableNormalAttrib(NORMAL_IN_NAME);
	voxelShader.deactivate();

	// Index VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLE_COUNT*sizeof(GLuint)*3, indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
	printf("VAO setup done! vao is %d\n", vao);
	setupModel();
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

	// Unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Terrain::setupModel() {
	// Start with filling the arrays

	modelVertexArray = (GLfloat*)malloc(MODEL_VERTEX_COUNT * 3 * sizeof(GLfloat));
	modelIndexArray = (GLuint*)malloc(MODEL_TRIANGLE_COUNT * 3 * sizeof(GLuint));

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

	// Fill up the VAO
	modelShader.activate();
	glGenVertexArrays(1, &modelVAO);
	glBindVertexArray(modelVAO);
	glGenBuffers(1, &modelVertexVBO);
	glGenBuffers(1, &modelIndexVBO);

	glBindBuffer(GL_ARRAY_BUFFER, modelVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, MODEL_VERTEX_COUNT * 3 * sizeof(GLfloat), modelVertexArray, GL_STATIC_DRAW);
	modelShader.setAndEnableVertexAttrib("inPosition");

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MODEL_TRIANGLE_COUNT * 3 * sizeof(GLuint), modelIndexArray, GL_STATIC_DRAW);

	modelShader.deactivate();
	glBindVertexArray(0);
}
