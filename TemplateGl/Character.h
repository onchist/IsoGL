#pragma once
#include "Entity.h"
#include "Model.h"
#include "Cell.h"

enum Team{RED, BLUE};

class Character : public Entity {
public:
	Character(Shader* program, Model* model, Cell* cellOn);
	~Character();

	virtual void draw();


	void offsetScale(glm::vec3 offsetScale);

	void setScale(glm::vec3 scale);
	std::string getName();

	glm::vec3 getScale() const;
	int getX() const;
	int getY() const;
	Team getTeam()const;
	void setSelected(bool selected);
	void setCellOn(Cell* cellOn);
	void setTeam(Team team);

protected:
	void update();
	Team _team;
	Cell* _cellOn;
	std::string _name;
	bool _selected;
	Model* _model;
	glm::vec3 _scale;
	float _rotX;
	float _rotY;
	float _rotZ;
};