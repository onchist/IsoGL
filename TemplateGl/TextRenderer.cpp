#include "TextRenderer.h"



TextRenderer::TextRenderer()
{
}


TextRenderer::~TextRenderer()
{
}

void TextRenderer::init(std::string fontPath) {
	ft = new FT_Library;
	if (FT_Init_FreeType(ft)) {
		utilities::PrintError("could not initialize freeType");
	}
	if (FT_New_Face(*ft, fontPath.c_str(), 0, &face)) {
		utilities::PrintError("failed to load font");
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	for (GLubyte c = 0; c < 128; c++)
	{

		std::cout << (int)c << std::endl;
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		CharacterData character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, CharacterData>(c, character));
	}
}