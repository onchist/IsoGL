#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "utilities.h"

class Shader
{
public:
	Shader::Shader(int N, GLenum* types, std::string** paths, int* nFiles);
	~Shader();
	void use();
	GLuint getID();
private:
	GLuint createProgram(const int N, GLuint* shaderIDs);
	GLuint loadShader(GLenum type, const int N, std::string* shaderPath);
	GLuint _programID;
	std::string readFile(const std::string pathToFile);
};

