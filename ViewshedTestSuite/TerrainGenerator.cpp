#include "TerrainGenerator.h"


TerrainGenerator::TerrainGenerator()
{
	rotate2D = glm::mat2(1.3623, 1.7531, -1.7131, 1.4623);
	seed = 1.0f;

	MOD2x = 3.07965;
	MOD2y = 7.1235;
	MOD3x = MOD2x;
	MOD3y = MOD2y;
	MOD3z = 4.998784;
}

void TerrainGenerator::generate(VoxelContainer *voxels, GLfloat *vertexArray, GLfloat *normalArray, GLuint *indexArray, GLint tileSize, GLuint triCount, GLuint vertCount) {

	// Start with setting some constants
	GLint width = tileSize;
	GLint height = tileSize;

	// Vectors to hold vertex data as well as indices
	//indexArray = (GLuint *)malloc(sizeof(GLuint) * triCount * 3);
	//vertexArray = (GLfloat *)malloc(sizeof(GLfloat) * vertCount * 3);
	//normalArray = (GLfloat *)malloc(sizeof(GLfloat) * vertCount * 3);

	// Start with initializing voxelarray to 0's
	voxels->init();

	// Now loop over both x and z and set the vertices +  normals
	for (int x = 0; x < width; x++)
		for (int z = 0; z < height; z++) {
			GLfloat height = getHeight((GLfloat)x, (GLfloat)z);
			vertexArray[(x + z * width) * 3 + 0] = (GLfloat)x;
			vertexArray[(x + z * width) * 3 + 1] = height; // Height (this is 2.5D)
			vertexArray[(x + z * width) * 3 + 2] = (GLfloat)z;

			// Update the voxels
			voxels->setValue(x, floor(height), z, 1);

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
}

glm::vec3 TerrainGenerator::calcNormal(GLfloat x, GLfloat y, GLfloat z) {
	GLfloat offset = 0.01;

	glm::vec3 normal = { 0.0, y, 0.0 };
	glm::vec3 v1 = normal - glm::vec3(offset, getHeight(x + offset, z), 0.0);
	glm::vec3 v2 = normal - glm::vec3(0.0, getHeight(x, z - offset), -offset);
	normal = glm::cross(v1, v2);
	normal = glm::normalize(normal);

	return normal;
}

void TerrainGenerator::setSeed(GLfloat seedIn) {
	seed = seedIn;
}

GLfloat TerrainGenerator::calcNeutral(GLfloat x, GLfloat z) const {
	// There's some real magic numbers in here!
	// The Noise calls add large mountain ranges for more variation over distances...
	//vec2 pos = p*0.05;
	x *= 0.025;
	z *= 0.025;

	float w = (Noise1(x*.25, z*.25)*0.75 + .15);
	w = 66.0 * w * w;
	float f = .0;
	glm::vec2 tmp;
	for (int i = 0; i < 5; i++)
	{
		f += w * Noise1(x, z);
		w = -w * 0.4;	//...Flip negative and positive for varition
		tmp = rotate2D * glm::vec2(x, z);
		x = tmp.x;
		z = tmp.y;
	}
	float ff = Noise1(x*.002, z*.002);
	f += pow(abs(ff), 5.0)*275.;

	return f+12.0;
}

GLfloat TerrainGenerator::getHeight(const GLfloat x, const GLfloat z) const {
	//return 0.0;
	return calcNeutral(x, z);
}

GLfloat TerrainGenerator::Hash12(GLfloat pX, GLfloat pY) const
{
	double dummy;
	bool xneg = false, yneg = false;
	if (pX < 0.0)
		xneg = true;
	if (pY < 0.0)
		yneg = true;

	//pX = (pX / MOD2x) - ftoi_fast(pX / MOD2x);
	//pY = (pY / MOD2y) - ftoi_fast(pY / MOD2y);
	pX = modf(pX / MOD2x, &dummy);
	pY = modf(pY / MOD2y, &dummy);

	if (xneg)
		pX = 1.0 + pX;
	if (yneg)
		pY = 1.0 + pY;

	GLfloat offset = 19.19f;
	GLfloat res = pX*(pY + offset) + pY*(pX + offset);
	pX += res;
	pY += res;
	//return (pX * pY) - floorf(pX * pY);

	res = modf(pX*pY, &dummy);

	return res;
}

GLfloat TerrainGenerator::Noise1(const GLfloat x, const GLfloat y) const
{
	GLfloat pX = floorf(x);
	GLfloat pY = floorf(y);
	GLfloat fX = x - pX;// fract(x);
	GLfloat fY = y - pY;
	fX = fX*fX*(3.0f - 2.0f*fX);
	fY = fY*fY*(3.0f - 2.0f*fY);
	GLfloat n = pX + pY*57.0;

	GLfloat pX1 = pX + 1.0f;
	GLfloat pY1 = pY + 1.0f;

	float res = glm::mix(glm::mix(Hash12(pX, pY), Hash12(pX1, pY), fX),
		glm::mix(Hash12(pX, pY1), Hash12(pX1, pY1), fX), fY);
	return res;
}