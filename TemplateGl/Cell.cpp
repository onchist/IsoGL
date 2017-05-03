#include "Cell.h"

GLuint Cell::vaoCube;

float Cell::unity = 1.2f;

Cell::Cell(Shader * program, GLuint diffuseMap, GLuint specularMap, glm::vec3 position): Entity(program, position), _diffuseMap(diffuseMap), _specularMap(specularMap)
{
	
}

void Cell::draw(){
	

	_program->use();

	glm::mat4 model; 
	model = glm::translate(model, _position);
	GLuint modelLoc = glGetUniformLocation(_program->getID(), "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _diffuseMap);
	glUniform1i(glGetUniformLocation(_program->getID(), "material.texture_diffuse1"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _specularMap);
	glUniform1i(glGetUniformLocation(_program->getID(), "material.texture_specular1"), 1);

	glBindVertexArray(Cell::vaoCube);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}