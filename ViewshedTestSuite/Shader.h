#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include<string>

// Abstraction for using shaders, handles compiling, using etc
class Shader
{
public:
	Shader();
	Shader(const std::string VS, const std::string FS); // Constructor that directly takes vs + fs strings
	~Shader() = default; // Nothing worth overriding the compiler-generated destructor for

	void loadFromFile(const std::string, const std::string); // Load vertex + frag shader from a string and sets it
	void activate() const; // Sets this particular shader as the current active one
	void deactivate() const; // Unsets this as the current shader

	void uploadMatrix(glm::mat4, std::string) const; // Upload a matrix to the shader
	void uploadMatrix(glm::mat3, std::string) const; // Overload of above method

	void uploadTexture(GLuint, std::string) const;

	void uploadBool(GLboolean, std::string) const;

	void uploadVec(glm::vec3, std::string) const;

	void setAndEnableVertexAttrib(const std::string);
	void setAndEnableNormalAttrib(const std::string);

private:
	// Member variables
	GLuint id; // The OpenGL context shader id of this particular shader
};

