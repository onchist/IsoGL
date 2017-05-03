#include "Character.h"



Character::Character(Shader * program, Model* model, Cell* cellOn) : Entity(program), _model(model), _scale(glm::vec3(0.0f))
{
	_program = program;
	_model = model;
	_position = glm::vec3(0.0f);
	_scale = glm::vec3(1.0f);
	_cellOn = cellOn;
	_cellOn->linkCharacter(this);
}




Character::~Character()
{
}

void Character::update() {
	_position = _cellOn->getTopPos();
}

void Character::draw()
{
	update();
	glUniform1i(glGetUniformLocation(_program->getID(), "uniColored"), true);
	glm::mat4 model;
	model = glm::translate(model, _position);
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.10f));

	model = glm::scale(model, _scale);
	model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(_program->getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));

	_model->draw(_program);
	glUniform1i(glGetUniformLocation(_program->getID(), "uniColored"), false);

}



void Character::offsetScale(glm::vec3 offsetScale)
{
	_scale += offsetScale;
}

void Character::setScale(glm::vec3 scale)
{
	_scale = scale;
}




glm::vec3 Character::getScale() const
{
	return _scale;
}
