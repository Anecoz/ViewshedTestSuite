#pragma once
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <string>

// Handles loading and saving to file
class FileHandler
{
public:
	FileHandler();
	~FileHandler() = default;

	static bool checkIfExists(const std::string& name); // Checks whether a file exists already
	static bool saveToFile(const std::string& name, GLfloat* arr, GLint size);
	static bool saveToFile(const std::string& name, GLuint* arr, GLint size);
	static GLfloat* loadFloatArrFromFile(const std::string& name);
	static GLuint* loadUintArrFromFile(const std::string& name);
};

