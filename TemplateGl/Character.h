#pragma once
#include "Entity.h"
#include "Model.h"
#include "Cell.h"

class Character : public Entity {
public:
	Character(Shader* program, Model* model, Cell* cellOn);
	~Character();

	virtual void draw();


	void offsetPos(glm::vec3 offsetPos);
	void offsetScale(glm::vec3 offsetScale);

	void setScale(glm::vec3 scale);
	void setPos(glm::vec3 pos);

	glm::vec3 getPos() const;
	glm::vec3 getScale() const;

protected:
	void update();
	Cell* _cellOn;
	Model* _model;
	glm::vec3 _scale;
};