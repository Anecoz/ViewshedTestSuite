#include "Shader.h"

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

// -------------UPLOADS------------
void Shader::uploadMatrix(glm::mat4 matrix, std::string name) const {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::uploadMatrix(glm::mat3 matrix, std::string name) const {
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}
