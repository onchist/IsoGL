#include "Character.h"



Character::Character(Shader * program, glm::vec3 position, Model* model) : Entity(program, position), _model(model), _scale(glm::vec3(0.0f))
{
	_program = program;
	_model = model;
	_position = glm::vec3(0.0f);
	_scale = glm::vec3(1.0f);
}




Character::~Character()
{
}

void Character::draw()
{
	glm::mat4 model;
	model = glm::translate(model, _position);
	model = glm::scale(model, _scale);
	model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(_program->getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));

	_model->draw(_program);

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
