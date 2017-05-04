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

enum class GameState{PLAY,EXIT};


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
	void draw();

	void update();

	void loadBoard();
	void initSystems();
	void freeSystems();
	void gameLoop();
	void pollEvents();
	void genVaos();
	void drawInfoTab();
		
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

	std::map<int,bool> _inputArray;
	std::map<int, bool> _firstInput;
	
	isoCamera _isoCamera;

	std::vector<Entity*> _entities;
	std::vector<Cell*> _board;
	int _boardX;
	int _boardY;

	int _focusX;
	int _focusY;

	int _keyPressed;

	float _fps;

	std::vector <Light*> _lights;
};

