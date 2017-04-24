#pragma once

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>
#include <iostream>
#include "utilities.h"
#include "Shader.h"
#include <math.h>
#include "isoCamera.h"
#include "Entity.h"
#include <vector>

enum class GameState{PLAY,EXIT};

class Game
{
public:
	Game();
	~Game();

	void run();
	
	

private:
	void draw();
	void update();
	void loadBoard();
	void initSystems();
	void freeSystems();
	void gameLoop();
	void pollEvents();
	void genVaos();
	SDL_Window* _window;
	int _screenWidth;
	int _screenHeight;
	GameState _gameState;
	SDL_GLContext _glContext;
	GLuint _vaoCube;
	GLuint _vaoSprite;
	GLuint _vaoLight;
	float _time;
	Shader* _program;
	Shader* _lightProgram;
	GLuint _diffuseMap;
	GLuint _specularMap;
	bool inputArray[1024];
	bool _firstInput[1024];
	float _lastFrame;
	float _deltaTime;
	isoCamera _isoCamera;
	Entity*** _board;
	Entity*** _entities;
	int _boardX;
	int _boardY;
	bool _keyPressed;
	int* _ptrCoord;
	glm::vec3 _lampPosition;
};

