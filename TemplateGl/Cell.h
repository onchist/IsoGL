#pragma once
#include "Entity.h"
#include <SDL/SDL.h>

class Character;



class Cell : public Entity {
public:
	Cell(Shader* program, GLuint diffuseMap, GLuint specularMap, int x, int y, bool walkable = true);
	virtual void draw();
	static GLuint vaoCube;
	glm::vec3 getTopPos();
	void linkCharacter(Character* character);
	void unlinkCharacter();
	Character* getCharacterPtr();
	glm::vec3 getPosition();
	bool holdsUnit();
	bool isWalkable();
	void setFocused(bool isFocused);
	void setReachable(bool isReachable);
	int getX();
	int getY();
	static float unity;
protected:
	bool _walkable;
	bool _hasUnitHeld;
	void update();
	GLuint _diffuseMap;
	GLuint _specularMap;
	int _x;
	int _y;
	float _height;
	Character* _characterHeld;
	bool _focused;
	bool _reachable;
};