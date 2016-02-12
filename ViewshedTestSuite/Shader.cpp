#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>

// Use the GL_utilities by Ingemar to perform shader loading
extern "C" //Ingemar's (useful) C headers
{
#include "GL_utilities.h"
}


Shader::Shader()
{
	// Uninitialized shader... might actually be dangerous
	id = 0;
}

Shader::Shader(const std::string vertexFileName, const std::string fragFileName) {
	// Simply load it directly from file
	loadFromFile(vertexFileName, fragFileName);
}

void Shader::loadFromFile(const std::string vs, const std::string fs) {
	// Use Ingemars loadshader, returns an id 
	id = loadShaders(vs.c_str(), fs.c_str());
}

void Shader::activate() const {
	glUseProgram(id);
}

void Shader::deactivate() const {
	glUseProgram(0);
}

void Shader::setAndEnableVertexAttrib(const std::string name) {
	GLint loc = glGetAttribLocation(id, name.c_str());

	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc);
	//printf("Enabled attrib %s, loc is %d\n", name.c_str(), loc);
}

void Shader::setAndEnableNormalAttrib(const std::string name) {
	GLint loc = glGetAttribLocation(id, name.c_str());

	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc);
	//printf("Enabled attrib %s, loc is %d\n", name.c_str(), loc);
}

// -------------UPLOADS------------
void Shader::uploadVec(glm::vec3 vec, std::string name) const {
	GLint loc = glGetUniformLocation(id, name.c_str());
	glUniform3fv(loc, 1, glm::value_ptr(vec));
	//printf("Uploaded vec (%f, %f, %f) to shader. loc is %d\n", vec.x, vec.y, vec.z, loc);
}

void Shader::uploadBool(GLboolean val, std::string name) const {
	GLint loc = glGetUniformLocation(id, name.c_str());
	glUniform1i(loc, val);
}

void Shader::uploadTexture(GLuint texUnit, std::string name) const {
	GLint loc = glGetUniformLocation(id, name.c_str());
	glUniform1i(loc, texUnit);
}

void Shader::uploadMatrix(glm::mat4 matrix, std::string name) const {
	GLint loc = glGetUniformLocation(id, name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, &matrix[0][0]);
	//printf("Uploaded matrix with name %s, loc is %d\n", name.c_str(), loc);
}

void Shader::uploadMatrix(glm::mat3 matrix, std::string name) const {
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}
