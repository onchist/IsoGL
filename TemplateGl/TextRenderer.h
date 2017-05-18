#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "utilities.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include <map>

struct CharacterData {
	GLuint TextureID;
	glm::vec2 Size;
	glm::vec2 Bearing;
	GLuint Advance;
};

class TextRenderer
{
public:
	TextRenderer();
	~TextRenderer();
	void initSystem();
	void init(std::string fontPath);
	void printText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
	static bool started;
	static Shader* textProgram;
	static GLuint VAO;
	static GLuint VBO;

	FT_Library ft;
	FT_Face face;
	std::map <GLchar, CharacterData> Characters;
};

