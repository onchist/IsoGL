#include "Game.h"

using namespace std;

float Game::_time = 0.0f;
float Game::_deltaTime = 0.0f;
float Game::_lastFrame = 0.0f;

//constructeur, initialise les attributs
Game::Game() {
	_window = nullptr;
	_screenWidth = 1024;
	_screenHeight = 576;
	_gameState = GameState::PLAY;
	
	_isoCamera = isoCamera();
	_focusX = 0;
	_focusY = 0;
	_keyPressed = 0;
}


Game::~Game() {

}


void Game::run() {
	//fonction qui se charge de tout initialiser
	initSystems();
	//lance la boucle de jeu
	gameLoop();
	//fait le menage
	freeSystems();
}

void Game::initSystems() {
	//initialise toute la sdl
	SDL_Init(SDL_INIT_EVERYTHING);

	// créé une fenetre et mets son adresse dans _window
	_window = SDL_CreateWindow("Window title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);
	//error checking
	if (_window == nullptr) {
		//insert error handling
	}
	SDL_SetRelativeMouseMode(SDL_TRUE);
	//création du contexte openGl avec notre fenetre et error checking
	_glContext = SDL_GL_CreateContext(_window);
	if (_glContext == nullptr) {
		//insert error handling
	}
	//pour faire fonctionner opengl > 3
	glewExperimental = true;

	//initialisation de glew et error checking
	if (glewInit() != GLEW_OK) {
		//insert error handling
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	//mise en place du double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SDL_GL_SetSwapInterval(1);
	//couleur de fond d'écran au glClear()
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Playground starts

	genVaos();

	int n = 2;
	GLenum* types = new GLenum[2]{ GL_VERTEX_SHADER,GL_FRAGMENT_SHADER };
	std::string** paths = new std::string*[n];
	paths[0] = new std::string[1]{ "shaders/vertex.glsl" };
	paths[1] = new std::string[1]{ "shaders/fragment.glsl" };
	int* nFiles = new int[2] {1, 1};

	_program = new Shader(n, types, paths, nFiles);

	int nL = 2;
	GLenum* typesL = new GLenum[2]{ GL_VERTEX_SHADER,GL_FRAGMENT_SHADER };
	std::string** pathsL = new std::string*[nL];
	pathsL[0] = new std::string[1]{ "shaders/lightVertex.glsl" };
	pathsL[1] = new std::string[1]{ "shaders/lightFragment.glsl" };
	int* nFilesL = new int[2]{ 1, 1 };

	_lightProgram = new Shader(nL, typesL, pathsL, nFilesL);

	_diffuseMap = utilities::TextureFromFile("textures/diffuse.png");

	_specularMap = utilities::TextureFromFile("textures/specular.png");

	loadBoard();
}

void Game::freeSystems()
{
	SDL_GL_DeleteContext(_glContext);
	SDL_DestroyWindow(_window);
	SDL_Quit();
	delete _program;
	delete _lightProgram;
}

void Game::pollEvents() {
	SDL_Event evnt;
	//systeme de file d'attente d'events
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_KEYDOWN:
			if (!_inputArray[evnt.key.keysym.sym]) {
				_keyPressed++;
				_inputArray[evnt.key.keysym.sym] = true;
				_firstInput[evnt.key.keysym.sym] = true;
			}
			else {
				_firstInput[evnt.key.keysym.sym] = false;
			}
			break;
		case SDL_KEYUP:
			_keyPressed--;
			_inputArray[evnt.key.keysym.sym] = false;
			_firstInput[evnt.key.keysym.sym] = false;
			break;
		case SDL_MOUSEMOTION:
			break;
		}
	}
}

void Game::gameLoop() {
	while (_gameState != GameState::EXIT) {
		pollEvents();
		update();
		draw();
	}
}

void Game::draw() {
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	_program->use();
	
	int pointN = 0;
	int dirN = 0;
	int spotN = 0;
	
	for (int i = 0; i < _lights.size(); i++) {
		if (_lights[i]->getType() == "pointLights") {
			_lights[i]->processUniforms(_program, pointN);
			pointN++;
		}
		else if (_lights[i]->getType() == "dirLights") {
			_lights[i]->processUniforms(_program, dirN);
			dirN++;
		}
		else if (_lights[i]->getType() == "spotLights") {
			_lights[i]->processUniforms(_program, spotN);
			spotN++;
		}
	}


	GLint matShineLoc = glGetUniformLocation(_program->getID(), "material.shininess");
	glUniform1f(matShineLoc, 128.0f);
	
	glm::mat4 view;
	view = _isoCamera.getViewMatrix();

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)_screenWidth / (float)_screenHeight, 0.1f, 100.0f);

	GLuint viewLoc = glGetUniformLocation(_program->getID(), "view");
	GLuint projLoc = glGetUniformLocation(_program->getID(), "projection");


	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	
	for(int i = 0; i < _entities.size(); i++) {
		_entities[i]->draw();
	}

	for (int i = 0; i < _board.size(); i++) {
		_board[i]->draw();
	}

	drawInfoTab();

	SDL_GL_SwapWindow(_window);
}

void Game::update() {
	Game::_time = SDL_GetTicks();
	Game::_deltaTime = (_time - _lastFrame) / 1000;
	Game::_lastFrame = _time;
	_fps = 1.0f / _deltaTime;

	if (_inputArray[SDLK_ESCAPE]) {
		_gameState = GameState::EXIT;
	}

	if (_firstInput[SDLK_z]) {
		_firstInput[SDLK_z] = false;
		if (_focusY + 1 < _boardY) {
			_focusY++;
		}
	}
	if (_firstInput[SDLK_s]) {
		_firstInput[SDLK_s] = false;
		if (_focusY - 1 >= 0) {
			_focusY--;
		}
	}
	if (_firstInput[SDLK_d]) {
		_firstInput[SDLK_d] = false;
		if (_focusX + 1 < _boardX) {
			_focusX++;
		}
	}
	if (_firstInput[SDLK_q]) {
		_firstInput[SDLK_q] = false;
		if (_focusX - 1 >= 0) {
			_focusX--;
		}
	}

	if (_inputArray[SDLK_a]) {
		_isoCamera.rotate(Game::_deltaTime * 1.0f);
	}
	if (_inputArray[SDLK_e]) {
		_isoCamera.rotate(Game::_deltaTime * -1.0f);
	}

	_isoCamera.setTarget(_board[_focusX + _focusY * _boardX]->getTopPos());
}


void Game::loadBoard() {
	glm::vec3 light = glm::vec3(1.0f, 1.0f, 1.0f);
	LightMaterial material;
	material.ambient = light * 0.2f;
	material.diffuse = light * 0.7f;
	material.specular = light * 1.0f;

	glm::vec3 position = glm::vec3(-3.0f, -3.0f, 5.0f);

	float constant = 1.0f;
	float linear = 0.045f;
	float quadratic = 0.0075f;

	PointLight* pointLight = new PointLight(material, position, constant, linear, quadratic);

	_lights.push_back(pointLight);

	Model* ourModel = new Model("models/MaleLow/MaleLow.obj");

	_boardX = 3;
	_boardY = 3;

	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 0, 0));
	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 1, 0));
	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 2, 0));

	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 0, 1));
	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 1, 1));
	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 2, 1));

	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 0, 2));
	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 1, 2));
	_board.push_back(new Cell(_program, _diffuseMap, _specularMap, 2, 2));

	Character* character0 = new Character(_program, ourModel, _board[1 + _boardY * 1]);
	_entities.push_back(character0);
}

void Game::genVaos() {
	float textureX = (float)1 / 3;
	float textureY = (float)1 / 2;

	//a multi-textured cube
	 


	GLfloat vertices[] = {
		// Positions          // Normals           // Texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};




	GLuint vboID;

	//generate both vao and vbo
	glGenVertexArrays(1, &_vaoCube);
	glGenBuffers(1, &vboID);

	//bind the vao
	glBindVertexArray(_vaoCube);

	//bind the vbo to the vao
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	//buffer the data contained in triangle[] in the vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	//stuffy stuff
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	//enable the vbo
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(2);

	//unbind the vao
	glBindVertexArray(0);
	//unbind the vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Cell::vaoCube = _vaoCube;

	GLfloat verticesSprite[]{
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
	};

	GLuint vboSprite;

	glGenVertexArrays(1, &_vaoSprite);
	glGenBuffers(1, &vboSprite);

	glBindVertexArray(_vaoSprite);
	glBindBuffer(GL_ARRAY_BUFFER, vboSprite);

	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSprite), verticesSprite, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &_vaoLight);
	glBindVertexArray(_vaoLight);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void Game::drawInfoTab(){
	system("cls");
	std::cout << "InfoTab Begin : " << std::endl;

	if (_board[_focusX + _focusY * _boardX]->holdsUnit()) {
		std::cout << "Unit name : " << _board[_focusX + _focusY * _boardX]->_getCharacterPtr()->getName() << std::endl;
	}

	std::cout << "InfoTab Ends" << std::endl;
}


