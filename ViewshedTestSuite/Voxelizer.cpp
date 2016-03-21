#include "Voxelizer.h"
#include "Game.h"

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

GLuint Voxelizer::voxelize(glm::mat4& projMatrix, glm::mat4& camMatrix) {
	// We want to pass through our voxelizing shaders, which will write to the voxeltexture. The texture will then contain what we need

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the viewport to match the texture
	//glViewport(0, 0, WIDTH, HEIGHT);

	// Disable this stuff
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	shader.activate();
	terrainModel->prepare();

	// Uploads
	shader.uploadMatrix(mvpX, "MVPx");
	shader.uploadMatrix(mvpY, "MVPy");
	shader.uploadMatrix(mvpZ, "MVPz");
	shader.uploadMatrix(projMatrix, "projMatrix");
	shader.uploadMatrix(camMatrix, "camMatrix");
	shader.uploadInt(WIDTH, "voxelDim");
	shader.uploadInt(DEPTH, "voxelDepth");

	glBindImageTexture(0, voxelTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	shader.uploadTexture(0, "voxTex");

	// Render
	terrainModel->render();
	// Immediately call memory barrier to ensure that the texture is written to
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	shader.deactivate();

	// Reset viewport
	glViewport(0, 0, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// DEBUG
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxelTex);
	GLfloat *testArr = (GLfloat *)malloc(sizeof(GLfloat) * WIDTH * DEPTH * HEIGHT);
	glGetTextureImage(voxelTex, 0, GL_RGBA, GL_RGBA32F, WIDTH*HEIGHT*DEPTH*sizeof(GLfloat), testArr);
	printf("texture at 7, 11, 178 is: %d\n", testArr[HEIGHT*WIDTH * 178 + DEPTH * 11 + 7]);

	for (int x = 0; x < 512; x++)
	for (int y = 0; y < 128; y++)
	for (int z = 0; z < 512; z++) {
	printf("texture at %d, %d, %d is %f\n", x, y, z, testArr[HEIGHT*WIDTH*z + DEPTH*y + x]);
	}*/

	// Lastly return the texture
	return voxelTex;
}

void Voxelizer::setupTexture() {
	glGenTextures(1, &voxelTex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxelTex);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, WIDTH, HEIGHT, DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindImageTexture(0, voxelTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	glBindTexture(GL_TEXTURE_3D, 0);
}