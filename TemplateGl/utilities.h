#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <vector>
#include <SDL/SDL.h>
#include <SOIL/SOIL.h>

#ifndef UTILITIES
#define UTILITIES
namespace utilities {
	std::string utilReadFile(const std::string pathToFile);
	void PrintError(std::string errorMessage);
	GLint TextureFromFile(std::string path);
	GLint TextureFromFile(const char* path, std::string directory);
}
#endif