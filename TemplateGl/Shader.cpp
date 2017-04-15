#include "Shader.h"



Shader::Shader(int N, GLenum* types, std::string** paths, int* nFiles){
	GLuint* shaderIDs = new GLuint[N];
	for (int i = 0; i < N; i++) {
		shaderIDs[i] = loadShader(types[i], nFiles[i], paths[i]);
	}
	_programID = createProgram(N, shaderIDs);
}


Shader::~Shader(){
}

void Shader::use() {
	glUseProgram(_programID);
}

GLuint Shader::loadShader(GLenum type, const int N, std::string* shaderPaths) {
	bool aborted = false;
	//initialize the shader
	GLuint shaderID = glCreateShader(type);
	//array containing the content of the files (string)
	std::string *shaderCode = new std::string[N];
	//c_str of the shaderCode
	char** shaderCodeCstr = new char*[N];
	//array stocking the lenghts of the files
	GLint *lenght = new GLint[N];
	//for each file
	for (int i = 0; i < N; i++)
	{
		//load the content of the file into a string
		shaderCode[i] = readFile(shaderPaths[i]);
		//error handling in case opening fails
		if (shaderCode[i] == "") { std::cout << "problem loading the sources from " << shaderPaths[i] << std::endl; aborted = true; }
		//get the lenght of the content of the file
		lenght[i] = shaderCode[i].length();
		//convert to c_str in order to be used in glShaderSource()
		shaderCodeCstr[i] = (char*)shaderCode[i].c_str();
	}
	//if all sources loaded correctly
	if (!aborted) {
		//load the source in the shader
		glShaderSource(shaderID, N, shaderCodeCstr, lenght);
		//compile
		glCompileShader(shaderID);
		//success checking
		int success = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		std::cout << "returned by GL_COMPILE_STATUS : " << success << std::endl;
	}
	return shaderID;
}

std::string Shader::readFile(const std::string pathToFile)
{
	std::ifstream t(pathToFile);
	if (t) { std::cout << "Stream with the file " << pathToFile << " succesfully openned." << std::endl; }

	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

GLuint Shader::createProgram(const int N, GLuint* shaderIDs) {
	GLuint programID;
	programID = glCreateProgram();
	for (int i = 0; i < N; i++) {
		glAttachShader(programID, shaderIDs[i]);
	}
	glLinkProgram(programID);
	GLint success = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		PrintError("Error while linking a shader program.");
		return 0;
	}
	return programID;
}

GLuint Shader::getID() {
	return _programID;
}