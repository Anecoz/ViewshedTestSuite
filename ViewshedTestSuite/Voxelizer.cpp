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

GLuint& Voxelizer::voxelize() {
	// We want to pass through our voxelizing shaders, which will write to the voxeltexture. The texture will then contain what we need

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the viewport to match the texture
	glViewport(0, 0, WIDTH, HEIGHT);

	// Disable this stuff
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	shader.activate();
	terrainModel->prepare();

	// Uploads
	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, voxelTex);
	glBindImageTexture(0, voxelTex, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8UI);
	shader.uploadTexture(0, "voxTex");
	shader.uploadMatrix(mvpX, "MVPx");
	shader.uploadMatrix(mvpY, "MVPy");
	shader.uploadMatrix(mvpZ, "MVPz");
	shader.uploadInt(WIDTH, "voxelWidth");
	shader.uploadInt(HEIGHT, "voxelHeight");
	shader.uploadInt(DEPTH, "voxelDepth");	

	// Render
	terrainModel->render();
	// Immediately call memory barrier to ensure that the texture is written to
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// DEBUG
	//glBindTexture(GL_TEXTURE_3D, voxelTex);
	//GLfloat *testArr = (GLfloat *)malloc(sizeof(GLfloat) * WIDTH * DEPTH * HEIGHT*4);
	
	/*GLubyte *testArr = new GLubyte[WIDTH*HEIGHT*DEPTH];
	printError("Before get text image");
	glGetTextureImage(voxelTex, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, WIDTH*HEIGHT*DEPTH*sizeof(GLubyte), testArr);

	printError("gettext image");
	printf("texture at 100, 50, 100 is: %d\n", testArr[0]);

	GLubyte max = 0;
	for (int x = 0; x < 512; x++)
	for (int y = 0; y < 128; y++)
	for (int z = 0; z < 512; z++) {
		GLubyte val = testArr[HEIGHT*WIDTH*z + WIDTH*y + x];
		if (val > max) {
			max = val;
			printf("curr max is %d\n", max);
		}		
	}
	delete[](testArr);*/

	shader.deactivate();

	// Reset viewport
	glViewport(0, 0, Game::WINDOW_SIZE_X, Game::WINDOW_SIZE_Y);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Lastly return the texture
	return voxelTex;
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

	GLubyte *testArr = new GLubyte[WIDTH*HEIGHT*DEPTH];
	std::fill(testArr, testArr + WIDTH*HEIGHT*DEPTH, 0);

	//glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, WIDTH, HEIGHT, DEPTH);// , 0, GL_RED, GL_FLOAT, NULL);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R8UI, WIDTH, HEIGHT, DEPTH, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, testArr);
	//glBindImageTexture(0, voxelTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

	delete[](testArr);

	glBindTexture(GL_TEXTURE_3D, 0);
}