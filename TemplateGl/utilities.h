#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <vector>
#include <SDL/SDL.h>

#ifndef UTILITIES
#define UTILITIES
std::string utilReadFile(const std::string pathToFile);
void PrintError(std::string errorMessage);
#endif