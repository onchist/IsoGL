#pragma once
#include "Entity.h"
#include "Model.h"
#include "Cell.h"

enum Team{RED, BLUE};

const int N_TEAM = 2;

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
	bool isUsed();
	void setUsed(bool used);
	int getReach();
protected:
	void update();
	Team _team;
	Cell* _cellOn;
	std::string _name;
	bool _selected;
	bool _used;

	int _reach;

	Model* _model;
	glm::vec3 _scale;
	float _rotX;
	float _rotY;
	float _rotZ;

};