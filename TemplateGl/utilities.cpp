#include "utilities.h"


//returns a string of the text file
std::string utilReadFile(const std::string pathToFile)
{
	std::ifstream t(pathToFile);
	if (t) { std::cout << "Stream with the file " << pathToFile << " succesfully openned." << std::endl; }
	
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

//simple error printing
void PrintError(std::string errorMessage) {
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
		"Fatal Error",
		errorMessage.c_str(),
		NULL);

}

