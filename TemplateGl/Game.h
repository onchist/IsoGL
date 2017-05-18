#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>
#include <iostream>
#include <map>
#include "utilities.h"
#include "Shader.h"
#include <math.h>
#include "isoCamera.h"
#include "Entity.h"
#include <vector>
#include "Model.h"
#include "Light.h"
#include "Character.h"
#include "Cell.h"
#include <stdlib.h>
#include <queue>

enum GameState{FREE, SELECTED, TARGETING, EXIT};


class Game
{
public:
	Game();
	~Game();

	void run();

	static float _time;
	static float _lastFrame;
	static float _deltaTime;

private:
	void removeCharacter(Character* character);
	int _turns;
	Team _playerTeam;
	void draw();

	float _timeTab;

	void update();

	void genReachMap(Cell* start, int reach, bool fly = true);
	void clearReachMap();
	Cell* getCell(int x, int y);

	void loadBoard();
	void initSystems();
	void freeSystems();
	void gameLoop();
	void pollEvents();
	void genVaos();
	void drawInfoTab();
	void nextTurn();
	void updateTime();
	void updateFps();
	void selectCharacter(Character* character);
	void unselectCharacter();

	std::vector<Cell*> getNeighbors(Cell* cell);
	void highlightCells();
	void clearCells();
		
	SDL_Window* _window;
	int _screenWidth;
	int _screenHeight;

	GameState _gameState;
	SDL_GLContext _glContext;
	GLuint _vaoCube;
	GLuint _vaoSprite;
	GLuint _vaoLight;



	Shader* _program;
	Shader* _lightProgram;

	GLuint _diffuseMap;
	GLuint _specularMap;
	Character* _selectedCharacter;
	Cell* _selectedCell;
	std::map<int,bool> _inputArray;
	std::map<int, bool> _firstInput;

	std::map<Cell*, bool> _reachMap;
	
	isoCamera _isoCamera;
	float _zoom;

	std::vector<Character*> _entities;
	std::vector<Cell*> _board;
	int _boardX;
	int _boardY;

	int _focusX;
	int _focusY;

	int _keyPressed;

	int* _nTypes;
	int* _nTypesLeft;

	float _fps;

	std::vector <Light*> _lights;
};

