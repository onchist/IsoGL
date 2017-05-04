#include "Cell.h"

GLuint Cell::vaoCube;

float Cell::unity = 1.2f;

Cell::Cell(Shader * program, GLuint diffuseMap, GLuint specularMap, int x, int y) : Entity(program), _diffuseMap(diffuseMap), _specularMap(specularMap), _x(x), _y(y)
{
	_height = 0.5f;
	_hasUnitHeld = false;
	_characterHeld = nullptr;
}

void Cell::update(){
	_position.x = _x * Cell::unity;
	_position.y = _y * Cell::unity;
	_position.z = 0.0f;
}

void Cell::draw(){
	update();

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

glm::vec3 Cell::getTopPos()
{
	update();
	return glm::vec3(_position.x, _position.y, _position.z + _height);
}

void Cell::linkCharacter(Character* character)
{
	_characterHeld = character;
	_hasUnitHeld = true;
}

void Cell::unlinkCharacter()
{
	_characterHeld = nullptr;
	_hasUnitHeld = false;
}

Character * Cell::_getCharacterPtr()
{
	return _characterHeld;
}

glm::vec3 Cell::getPosition()
{
	update();
	return _position;

}

bool Cell::holdsUnit()
{
	return _hasUnitHeld;
}