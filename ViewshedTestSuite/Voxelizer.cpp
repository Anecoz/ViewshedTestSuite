#include "Voxelizer.h"
#include "Game.h"
#include "SVO.h"

extern "C" {
#include "GL_utilities.h"
}


Voxelizer::Voxelizer()
{
	// Dangerous to set shader immediately here, since we are not guaranteed to have an OpenGL context yet
}

void Voxelizer::init(DrawableModel* terrainModel) {
	// Setting shaders here should be fine, the context should be set
	shader = Shader("voxelizer.vert", "voxelizer.frag", "voxelizer.geom");
	printError("after shader init");
	this->terrainModel = terrainModel;
	setupTexture();
	printError("after setup texture");
	this->terrainModel->addShader(shader);
	printError("after addShader");
}

GLuint Voxelizer::getVoxelListSize() {
	calcVoxelList(false);

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);

	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, voxelCounterAtomicBuffer);
	GLuint* count = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
	
	GLuint size = count[0];
	
	// Reset the counter
	memset(count, 0, sizeof(GLuint));
	return size;
}

void Voxelizer::calcVoxelList(GLboolean shouldStoreVoxels) {
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the viewport to match the texture
	glViewport(0, 0, WIDTH, DEPTH);

	// Disable this stuff
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	shader.activate();
	terrainModel->prepare();

	if (shouldStoreVoxels) {
		glBindImageTexture(0, voxelPosTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGB10_A2UI);
		shader.uploadTexture(0, "voxelPos");
	}

	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, voxelCounterAtomicBuffer);
	shader.uploadMatrix(mvpX, "MVPx");
	shader.uploadMatrix(mvpY, "MVPy");
	shader.uploadMatrix(mvpZ, "MVPz");
	shader.uploadInt(WIDTH, "voxelDim");	// width, height and depth are the same, so choose one	
	shader.uploadBool(shouldStoreVoxels, "shouldStoreVoxels");
	
	terrainModel->render();

	// Immediately call memory barrier to ensure that the texture is written to
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	shader.deactivate();

	// Reset viewport
	glViewport(0, 0, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

GLuint& Voxelizer::voxelize() {
	// Pass through our voxelization shaders once to get number of voxels for our voxel list
	// Once we have this, allocate a buffer and rerun shaders
	// Store each fragment in the voxel list

	// Generate the atomic counter that will get the number of voxels
	genAtomicBuffer(voxelCounterAtomicBuffer);
	GLuint size = getVoxelListSize();
	printf("Number of voxels in list: %d\n", size);
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	// Generate buffer that will hold the voxel list
	genLinearBuffer(sizeof(GLuint) * size, GL_R32UI, &voxelPosTex, &voxelPosTBO);

	// Voxelize again but this time store the voxels in the list
	calcVoxelList(true);

	// Copy buffer to CPU memory
	GLubyte *voxelBufferList = new GLubyte[4*size];	// RGBA values
	glBindBuffer(GL_TEXTURE_BUFFER, voxelPosTBO);
	glGetBufferSubData(GL_TEXTURE_BUFFER, 0, 4*size*sizeof(GLubyte), voxelBufferList);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// We now have a list of every voxel world position in the linear buffer voxelPosTex
	// Build SVO out of this information
	buildSVO(voxelBufferList, size);
	
	// Lastly return the texture
	printf("Voxelisation done!\n");
	return voxelTex;
}

void Voxelizer::buildSVO(GLubyte* voxelBufferList, GLuint size) {
	printf("Starting SVO build...\n");

	SVO svo(WIDTH, glm::vec3(WIDTH/2, HEIGHT/2, DEPTH/2));
	// Loop through the buffer list and build upon SVO
	for (int i = 0; i < size; i++) {
		glm::vec3 currVoxelPos;
		currVoxelPos.x = voxelBufferList[i * 4 + 0];
		currVoxelPos.y = voxelBufferList[i * 4 + 1];
		currVoxelPos.z = voxelBufferList[i * 4 + 2];

		svo.insert(currVoxelPos);
	}
	printf("SVO build complete!\n");
	
	delete[](voxelBufferList);
}

void Voxelizer::genLinearBuffer(GLint size, GLenum format, GLuint* tex, GLuint* tbo)
{
	if ((*tbo) > 0)
		glDeleteBuffers(1, tbo);  //delete previously created tbo

	glGenBuffers(1, tbo);

	glBindBuffer(GL_TEXTURE_BUFFER, *tbo);
	glBufferData(GL_TEXTURE_BUFFER, size, 0, GL_STATIC_DRAW);

	if ((*tex) > 0)
		glDeleteTextures(1, tex); //delete previously created texture

	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_BUFFER, *tex);
	glTexBuffer(GL_TEXTURE_BUFFER, format, *tbo);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}

void Voxelizer::genAtomicBuffer(GLuint &buffer) {
	GLuint initVal = 0;

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, buffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), &initVal, GL_STATIC_DRAW);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
}

void Voxelizer::setupTexture() {
	glGenTextures(1, &voxelTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxelTex);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//GLubyte *testArr = new GLubyte[WIDTH*HEIGHT*DEPTH];
	//std::fill(testArr, testArr + WIDTH*HEIGHT*DEPTH, 0);

	//glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, WIDTH, HEIGHT, DEPTH);// , 0, GL_RED, GL_FLOAT, NULL);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, WIDTH, HEIGHT, DEPTH, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
	//glBindImageTexture(0, voxelTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

	//delete[](testArr);

	glBindTexture(GL_TEXTURE_3D, 0);
}