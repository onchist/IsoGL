#include "Entity.h"



Entity::Entity(Shader* programID, GLuint diffuseMap, GLuint specularMap, GLuint vaoID, glm::vec3 position) {
	_programID = programID;
	_diffuseMap = diffuseMap;
	_specularMap = specularMap;
	_vaoID = vaoID;
	_position = position;
}



Entity::~Entity()
{
}

void Entity::draw(glm::mat4 view, glm::mat4 projection)
{
	glUniformMatrix4fv(glGetUniformLocation(_programID->getID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(_programID->getID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _diffuseMap);
	glUniform1i(glGetUniformLocation(_programID->getID(), "material.diffuse"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _specularMap);
	glUniform1i(glGetUniformLocation(_programID->getID(), "material.specular"), 1);


	_programID->use();
	glBindVertexArray(_vaoID);


	glm::mat4 model;
	model = glm::translate(model, _position);
	GLfloat angle = 0.0f;
	model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(_programID->getID(), "model"), 1, GL_FALSE, glm::value_ptr(model));
	
	glDrawArrays(GL_TRIANGLES, 0, 36);
	
	glBindVertexArray(0);
}

void Entity::move(glm::vec3 offset)
{
	_position += offset;
}

glm::vec3 Entity::getPos()
{
	return _position;
}
