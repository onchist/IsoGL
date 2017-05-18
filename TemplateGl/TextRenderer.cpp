#include "TextRenderer.h"

bool TextRenderer::started = false;
Shader* TextRenderer::textProgram = nullptr;
GLuint TextRenderer::VAO = 0;
GLuint TextRenderer::VBO = 0;


TextRenderer::TextRenderer()
{
}


TextRenderer::~TextRenderer()
{
}

void TextRenderer::init(std::string fontPath) {
	if (!started) {
		initSystem();
	}

	if (FT_Init_FreeType(&ft)) {
		utilities::PrintError("could not initialize freeType");
	}
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// Now store character for later use
		CharacterData character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, CharacterData>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::printText(std::string text, float x, float y, float scale, glm::vec3 color) {
	textProgram->use();
	glUniform3f(glGetUniformLocation(textProgram->getID(), "textColor"), color.x, color.y, color.z);
	glm::mat4 projection;
	projection = glm::ortho(0, 1024, 0, 576);
	glUniformMatrix4fv(glGetUniformLocation(textProgram->getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		CharacterData ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		x += (ch.Advance >> 6) * scale;

	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::initSystem() {
	int nT = 2;
	GLenum* typesT = new GLenum[2]{ GL_VERTEX_SHADER,GL_FRAGMENT_SHADER };
	std::string** pathsT = new std::string*[nT];
	pathsT[0] = new std::string[1]{ "shaders/textVertex.glsl" };
	pathsT[1] = new std::string[1]{ "shaders/textFragment.glsl" };
	int* nFilesT = new int[2]{ 1, 1 };

	textProgram = new Shader(nT, typesT, pathsT, nFilesT);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	TextRenderer::started = true;
}