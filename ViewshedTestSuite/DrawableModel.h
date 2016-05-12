#pragma once
#include "Shader.h"
#include <String>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>

typedef std::vector<Shader> ShaderList;

/*
Holds normal models
*/
class DrawableModel
{
public:
	DrawableModel();
	DrawableModel(GLfloat *vertexArr, GLfloat *normalArr, GLuint *indexArr);
	~DrawableModel();

	void init(ShaderList &shaders, GLuint vertCount, GLuint triCount);

	void prepare();
	void render();
	void renderPoints();
	void addShader(Shader& shader);

	// MEMBER OBJECTS
	ShaderList shaders; // make this public so that we can set stuff from the outside

private:

	// MEMBER VARIABLES
	GLuint vao, vertexVBO, normalVBO, indexVBO;
	GLfloat *vertexArray, *normalArray;
	GLuint *indexArray;

	GLuint vertexCount, triangleCount;

	const std::string VERTEX_IN_NAME = "inPosition";
	const std::string NORMAL_IN_NAME = "inNormal";

	// MEMBER METHODS
	void setupVAO();
};

