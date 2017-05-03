#include "Entity.h"


Entity::Entity(Shader* program): _program(program), _position(glm::vec3(0.0f))
{

}



Entity::~Entity()
{
}

void Entity::offsetPos(glm::vec3 offsetPos)
{
	_position += offsetPos;
}

void Entity::setPos(glm::vec3 pos)
{
	_position = pos;
}

glm::vec3 Entity::getPos() const
{
	return _position;
}

