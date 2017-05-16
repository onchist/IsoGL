#pragma once
#include "Entity.h"
#include "Model.h"
#include "Cell.h"

enum Team{RED, BLUE};

const int N_TEAM = 2;

struct CharacterStats {
	std::string name;
	int atk;
	int hp;
	int mov;
	int atkrange;
	Team team;
};

class Character : public Entity {
public:
	Character(Shader* program, Model* model, Cell* cellOn, CharacterStats stats);
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
	int getAtkReach();
	Cell* getCellOn();
	void takeDamage(int damage);
	void attack(Character* character);
	int getHp();
	int getMaxHp();
	bool isAlive();
protected:
	bool _alive;
	void update();
	Team _team;
	Cell* _cellOn;
	std::string _name;
	int _hp;
	int _maxHp;
	int _atk;
	bool _selected;
	bool _used;

	int _reach;
	int _atkReach;

	Model* _model;
	glm::vec3 _scale;
	float _rotX;
	float _rotY;
	float _rotZ;

};