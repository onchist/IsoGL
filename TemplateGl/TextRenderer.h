#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "utilities.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
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
	void init(std::string fontPath);

private:
	FT_Library* ft;
	FT_Face face;
	std::map <GLchar, CharacterData> Characters;
};

