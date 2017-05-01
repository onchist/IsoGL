#include "Entity.h"



Entity::Entity(Shader * program, Model * model)
{
	_program = program;
	_model = model;
	_position = glm::vec3(0.0f);
	_scale = glm::vec3(1.0f);
}

Entity::Entity(Shader * program, Model * model, glm::vec3 position)
{
	_program = program;
	_model = model;
	_position = position;
	_scale = glm::vec3(1.0f);
}

Entity::Entity(Shader* program, Model* model, glm::vec3 position, glm::vec3 scale) {
	_program = program;
	_model = model;
	_position = position;
	_scale = scale;
}


Entity::~Entity()
{
}

void Entity::draw(glm::mat4 view, glm::mat4 projection)
{
	_program->use();
	glUniformMatrix4fv(glGetUniformLocation(_program->getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(_program->getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	
	glm::mat4 model;
	model = glm::translate(model, _position);
	model = glm::scale(model, _scale);
	model = glm::rotate(model, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(_program->getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	
	_model->draw(_program);
	
}



void Entity::offsetPos(glm::vec3 offsetPos)
{
	_position += offsetPos;
}

void Entity::offsetScale(glm::vec3 offsetScale)
{
	_scale += offsetScale;
}

void Entity::setScale(glm::vec3 scale)
{
	_scale = scale;
}

void Entity::setPos(glm::vec3 pos)
{
	_position = pos;
}

glm::vec3 Entity::getPos()
{
	return _position;
}

glm::vec3 Entity::getScale()
{
	return _scale;
}
