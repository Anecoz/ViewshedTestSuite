#include "DrawableModel.h"

extern "C" {
#include "GL_utilities.h"
}


DrawableModel::DrawableModel()
{
	vertexArray = nullptr;
	normalArray = nullptr;
	indexArray = nullptr;
}

DrawableModel::DrawableModel(GLfloat *vertexArray, GLfloat *normalArray, GLuint *indexArray) {
	this->vertexArray = vertexArray;
	this->normalArray = normalArray;
	this->indexArray = indexArray;
}

DrawableModel::~DrawableModel()
{
	free(vertexArray);
	free(normalArray);
	free(indexArray);

	// GL memory frees
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &indexVBO);

	// Not all models have normals
	if (normalArray != nullptr)
		glDeleteBuffers(1, &normalVBO);
}

void DrawableModel::init(ShaderList &shaders, GLuint vertexCount, GLuint triangleCount) {
	this->shaders = shaders;
	this->vertexCount = vertexCount;
	this->triangleCount = triangleCount;

	setupVAO();
}

void DrawableModel::prepare() {
	glBindVertexArray(vao);
}

void DrawableModel::render() {
	glDrawElements(GL_TRIANGLES, triangleCount * 3, GL_UNSIGNED_INT, 0L);
	glBindVertexArray(0);
}

void DrawableModel::addShader(Shader &shader) {
	shader.activate();
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	
	shader.setAndEnableVertexAttrib(VERTEX_IN_NAME);
	printError("after setandenablevertex");
	
	if (normalArray != nullptr) {
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		shader.setAndEnableNormalAttrib(NORMAL_IN_NAME);
		printError("after setnormals");
	}
	
	shader.deactivate();
}

void DrawableModel::setupVAO() {
	// Generate a new VAO as well as VBOs, and construct it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vertexVBO);
	if (normalArray != nullptr)
		glGenBuffers(1, &normalVBO);
	glGenBuffers(1, &indexVBO);

	// Vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), vertexArray, GL_STATIC_DRAW);

	for (Shader &shader : shaders) {
		shader.activate();
		shader.setAndEnableVertexAttrib(VERTEX_IN_NAME);
		shader.deactivate();
	}

	// Normal VBO
	if (normalArray != nullptr) {
		glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
		glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GLfloat), normalArray, GL_STATIC_DRAW);

		for (Shader &shader : shaders) {
			shader.activate();
			shader.setAndEnableNormalAttrib(NORMAL_IN_NAME);
			shader.deactivate();
		}		
	}	

	// Index VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleCount*sizeof(GLuint) * 3, indexArray, GL_STATIC_DRAW);

	glBindVertexArray(0);
}
