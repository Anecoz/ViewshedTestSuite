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
	f += pow(abs(ff), 5.0)*275. - 25.0;

	return f;
}

GLfloat TerrainGenerator::getHeight(const GLfloat x, const GLfloat z) const {
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