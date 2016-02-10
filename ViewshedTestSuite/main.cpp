// To get rid of VS complaining that printf is not secure enough
#define _CRT_SECURE_NO_DEPRECATE

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <iostream>

#include "Game.h"

extern "C" //Ingemar's (useful) C headers
{
#include "GL_utilities.h"
}

using namespace std;

// Link
#pragma comment(lib, "glew32.lib")


int main(int argc, char **argv) {
	
	int i;
	//std::cin >> i;
	Game game;
	game.run(argc, argv);
	std::cin >> i;
	return 0;
}