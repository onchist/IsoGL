#pragma once
#include "Entity.h"
#include <SDL/SDL.h>

class Character;



class Cell : public Entity {
public:
	Cell(Shader* program, GLuint diffuseMap, GLuint specularMap, int x, int y);
	virtual void draw();
	static GLuint vaoCube;
	glm::vec3 getTopPos();
	void linkCharacter(Character* character);
	void unlinkCharacter();
	Character* _getCharacterPtr();
	glm::vec3 getPosition();
	bool holdsUnit();
protected:
	bool _hasUnitHeld;
	void update();
	GLuint _diffuseMap;
	GLuint _specularMap;
	static float unity;
	int _x;
	int _y;
	float _height;
	Character* _characterHeld;
};