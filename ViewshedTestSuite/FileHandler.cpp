#include "FileHandler.h"
#include <fstream>
#include <sys\stat.h>
#include <vector>


FileHandler::FileHandler()
{
}

bool FileHandler::checkIfExists(const std::string& name) {
	struct stat buf;
	if (stat(name.c_str(), &buf) != -1)
	{
		return true;
	}
	return false;
}

bool FileHandler::saveToFile(const std::string& name, GLfloat* arr, GLint size) {
	std::ofstream myfile(name);
	if (myfile.is_open())
	{
		// Write the size first
		myfile << size << " ";

		for (int count = 0; count < size; count++) {
			myfile << arr[count] << " ";
		}
		myfile.close();
		return true;
	}
	else return false;
}

bool FileHandler::saveToFile(const std::string& name, GLuint* arr, GLint size) {
	std::ofstream myfile(name);
	if (myfile.is_open())
	{
		// Write the size first
		myfile << size << " ";

		for (int count = 0; count < size; count++) {
			myfile << arr[count] << " ";
		}
		myfile.close();
		return true;
	}
	else return false;
}

GLfloat* FileHandler::loadFloatArrFromFile(const std::string& name) {
	std::ifstream file(name);

	if (file.is_open()) {
		// Get the size first
		int size;
		file >> size;

		GLfloat* output = (GLfloat*)malloc(size*sizeof(GLfloat));
		int n = 0;
		while (file >> output[n]) {
			n++;
		}

		return output;
	}
	else return nullptr;	
}

GLuint* FileHandler::loadUintArrFromFile(const std::string& name) {
	std::ifstream file(name);

	if (file.is_open()) {
		// Get the size first
		int size;
		file >> size;

		GLuint* output = (GLuint*)malloc(size*sizeof(GLuint));
		int n = 0;
		while (file >> output[n]) {
			n++;
		}

		return output;
	}
	else return nullptr;
}