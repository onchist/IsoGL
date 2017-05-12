#include "Character.h"



Character::Character(Shader * program, Model* model, Cell* cellOn) : Entity(program), _model(model)
{
	_program = program;
	_model = model;
	_position = glm::vec3(0.0f);
	_scale = glm::vec3(0.1f);
	_cellOn = cellOn;
	_cellOn->linkCharacter(this);
	_rotX = glm::radians(90.0f);
	_rotY = glm::radians(0.0f);
	_rotZ = glm::radians(0.0f);
	_name = "placeholder";
	_team = BLUE;
	_used = false;
	_reach = 3;
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
	glm::vec3 color;
	if (_selected) {
		color = glm::vec3(0.0f, 0.5f, 0.0f);
	}
	else  if (_team == BLUE){
		color = glm::vec3(0.0f, 0.0f, 0.5f);
	}
	else  if (_team == RED) {
		color = glm::vec3(0.5f, 0.0f, 0.0f);
	}

	if (_used) {
		color *= 0.2f;
	}

	glUniform3f(glGetUniformLocation(_program->getID(), "uniColorDiffuse"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(_program->getID(), "uniColorSpecular"), 1.0f, 1.0f, 1.0f);

	glm::mat4 model;
	model = glm::translate(model, _position);

	model = glm::rotate(model, _rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, _rotY, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, _rotX, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, _scale);
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

std::string Character::getName()
{
	return _name;
}




glm::vec3 Character::getScale() const
{
	return _scale;
}

int Character::getX() const
{
	return _cellOn->getX();
}

int Character::getY() const
{
	return _cellOn->getY();
}

Team Character::getTeam() const
{
	return _team;
}

void Character::setSelected(bool selected)
{
	_selected = selected;
}

void Character::setCellOn(Cell * cellOn)
{
	_cellOn->unlinkCharacter();
	_cellOn = cellOn;
	_cellOn->linkCharacter(this);
}

void Character::setTeam(Team team)
{
	_team = team;
}

bool Character::isUsed()
{
	return _used;
}

void Character::setUsed(bool used)
{
	_used = used;
}

int Character::getReach()
{
	return _reach;
}
