#include "Game.h"

using namespace std;

float Game::_time = 0.0f;
float Game::_deltaTime = 0.0f;
float Game::_lastFrame = 0.0f;

//constructeur, initialise les attributs
Game::Game() {
	_timeTab = 0;
	_zoom = 1.0f;
	_window = nullptr;
	_screenWidth = 1024;
	_screenHeight = 576;
	_gameState = FREE;
	
	_isoCamera = isoCamera();
	_focusX = 0;
	_focusY = 0;
	_keyPressed = 0;
	_playerTeam = BLUE;
	_turns = 0;
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
	//SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN);
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
	int* nFiles = new int[2]{ 1, 1 };

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

void Game::removeCharacter(Character * character)
{
	character->getCellOn()->unlinkCharacter();
	_nTypes[character->getTeam()]--;
}

void Game::draw() {
	//on supprime les élements d'openGL relatifs à la trame précédente
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//on lance le shader qu'on a créé
	_program->use();

	//On passe au shader les sources de lumières présentes dans le jeu
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

	//on génere la matrice pour créer l'effet de caméra
	glm::mat4 view;
	view = _isoCamera.getViewMatrix(_zoom);

	//on génere la matrice pour créer la perspective
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)_screenWidth / (float)_screenHeight, 0.1f, 100.0f);


	//on envoie ces matrices au shader
	GLuint viewLoc = glGetUniformLocation(_program->getID(), "view");
	GLuint projLoc = glGetUniformLocation(_program->getID(), "projection");


	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//on affiche tous les personnages
	for (int i = 0; i < _entities.size(); i++) {
		if (_entities[i]->isAlive()) {
			_entities[i]->draw();
		}
	}

	//on affiche toutes les cases
	for (int i = 0; i < _board.size(); i++) {
		_board[i]->draw();
	}

	//on affiche dans la console une liste d'informations
	drawInfoTab();

	//on échange les trames (on est en double buffering, on dessine integralement
	//notre trame puis on échange les trames)
	SDL_GL_SwapWindow(_window);
}

void Game::update() {
	//on mets à jour l'écart de temps depuis la derniere trame
	updateTime();
	//on calcule les fps
	updateFps();

	//si ESC, on quite
	if (_inputArray[SDLK_ESCAPE]) {
		_gameState = GameState::EXIT;
	}

	//on dit à la case actuelle qu'elle n'est plus sélectionnée
	_board[_focusX + _focusY * _boardX]->setFocused(false);

	//changement de case sélectionnée
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
	if (_inputArray[SDLK_p]) {
		_zoom = _zoom + _deltaTime * 0.5f;
	}
	if (_inputArray[SDLK_m]) {

		_zoom = _zoom + _deltaTime * -0.5f;
	}
	//on désigne la nouvelle case comme sélectionnée
	_selectedCell = _board[_focusX + _focusY * _boardX];
	//si aucuns personnage n'est selectionnée,
	//et qu'il y a un personnage de la même équipe que le joueur sur la case,
	// on le sélectionne
	if (_gameState == FREE) {
		if (_firstInput[SDLK_e]) {
			_firstInput[SDLK_e] = false;

			if (_selectedCell->holdsUnit()) {
				Character* character = _selectedCell->getCharacterPtr();
				if (character->getTeam() == _playerTeam && !character->isUsed()) {
					selectCharacter(character);
				}
			}
		}
	}
	//on indique à la case sélectionnée qu'elle l'est
	_selectedCell->setFocused(true);
	//on mets à jour la caméra
	_isoCamera.setTarget(_selectedCell->getTopPos());

	//phase de mouvement du joueur
	if (_gameState == SELECTED) {
		if (_firstInput[SDLK_BACKSPACE]) {
			_firstInput[SDLK_BACKSPACE] = false;
			unselectCharacter();
		}
		if (_firstInput[SDLK_e]) {
			_firstInput[SDLK_e] = false;
			if (!_selectedCell->holdsUnit() || 
				_selectedCell == _selectedCharacter->getCellOn()) {
				if (_reachMap[_selectedCell]) {
					_selectedCharacter->setCellOn(_selectedCell);

					_gameState = TARGETING;
					clearReachMap();
					clearCells();
					//genReachMap génére un tableau qui associe 
					//à chaque case le fait d'etre attaquable ou non.
					genReachMap(_selectedCharacter->getCellOn(), 
						_selectedCharacter->getAtkReach(), true);
					highlightCells();
				}
			}
		}
	}
	//si on a déplacé le personnage, 
	//on peut le faire attaquer un personnage ennemi
	if (_gameState == TARGETING) {
		if (_firstInput[SDLK_BACKSPACE]) {
			_firstInput[SDLK_BACKSPACE] = false;
			_selectedCharacter->setUsed(true);
			_nTypesLeft[_playerTeam]--;
			unselectCharacter();
		}

		if (_firstInput[SDLK_e]) {
			_firstInput[SDLK_e] = false;
			if (_reachMap[_selectedCell] && _selectedCell->holdsUnit()) {
				if (_selectedCell->getCharacterPtr()->getTeam() != 
					_selectedCharacter->getTeam()) 
				{
					_selectedCharacter->attack(_selectedCell->getCharacterPtr());
					if (!_selectedCell->getCharacterPtr()->isAlive()) {
						removeCharacter(_selectedCell->getCharacterPtr());
					}
					_selectedCharacter->setUsed(true);
					_nTypesLeft[_playerTeam]--;
					unselectCharacter();
				}
			}
		}
	}
	//si tous les personnages du joueur sont "fatigués", on passe le tour et l'autre
	//équipe prend la main
	if (_nTypesLeft[_playerTeam] == 0) {
		nextTurn();
	}
}


void Game::clearReachMap()
{
	_reachMap.clear();
}

Cell * Game::getCell(int x, int y)
{
	if (x >= 0 && x < _boardX) {
		if(y >= 0 && y < _boardY){
			return _board[x + _boardX * y];
		}
	}
	return nullptr;
}

void Game::loadBoard() {
	_boardX = 12;
	_boardY = 12;

	int hardCodedBoard[] = { 
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
		1, 2, 1, 1, 3, 4, 3, 1, 1, 0, 1, 1,
		1, 1, 2, 1, 1, 1, 2, 1, 0, 1, 1, 0,
		1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0,
		1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1,
		1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1,
		0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1,
		1, 0, 1, 5, 1, 1, 5, 1, 5, 1, 0, 0,
		1, 6, 1, 1, 1, 7, 1, 6, 1, 1, 1, 0,
		1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	};

	glm::vec3 light = glm::vec3(1.0f, 1.0f, 1.0f);
	LightMaterial material;
	material.ambient = light * 0.2f;
	material.diffuse = light * 0.7f;
	material.specular = light * 1.0f;

	glm::vec3 position = glm::vec3(_boardX * Cell::unity/2, _boardY * Cell::unity/2, 10.0f);

	float constant = 1.0f;
	float linear = 0.027f;
	float quadratic = 0.0028f;

	PointLight* pointLight = new PointLight(material, position, constant, linear, quadratic);

	_lights.push_back(pointLight);

	for (int y = 0; y < _boardY; y++) {
		for (int x = 0; x < _boardX; x++) {
			switch (hardCodedBoard[x + y * _boardX]) {
			case 0:
				_board.push_back(new Cell(_program, _specularMap, _specularMap, x, y, false));
				break;
			default:
				_board.push_back(new Cell(_program, _diffuseMap, _specularMap, x, y));
				break;
			}
		}
	}

	
	Model* ourModel = new Model("models/MaleLow/MaleLow.obj");
	CharacterStats peonBleu = { "guerrier", 4, 10, 3, 1, BLUE };
	CharacterStats archerBleu = { "archer", 3, 7, 3, 2, BLUE };
	CharacterStats bossBleu = { "boss", 3, 15, 2, 1, BLUE };

	CharacterStats peonRouge = { "guerrier", 4, 10, 3, 1, RED };
	CharacterStats archerRouge = { "archer", 3, 7, 3, 2, RED };
	CharacterStats bossRouge = { "boss", 3, 15, 2, 1, RED };

	for (int y = 0; y < _boardY; y++) {
		for (int x = 0; x < _boardX; x++) {
			CharacterStats stats;
			bool presence = true;
			switch (hardCodedBoard[x + y * _boardX]) {
			case 2:
				stats = peonBleu;
				break;
			case 3:
				stats = archerBleu;
				break;
			case 4:
				stats = bossBleu;
				break;
			case 5:
				stats = peonRouge;
				break;
			case 6:
				stats = archerRouge;
				break;
			case 7:
				stats = bossRouge;
				break;
			default:
				presence = false;
				break;
			}
			if (presence) { _entities.push_back(new Character(_program, ourModel, _board[x + y * _boardX], stats)); }
		}
	}


	_nTypes = new int[N_TEAM];
	_nTypesLeft = new int[N_TEAM];
	for (int i = 0; i < N_TEAM; i++) {
		_nTypes[i] = 0;
		_nTypesLeft[i] = 0;
	}

	for (int i = 0; i < _entities.size(); i++) {
		_nTypes[_entities[i]->getTeam()]++;
		_nTypesLeft[_entities[i]->getTeam()]++;
	}
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

void Game::drawInfoTab() {
	_timeTab += _deltaTime;
	if (_timeTab >= 0.2f)
	{
		_timeTab = 0.0f;
		system("cls");
		std::cout << "InfoTab Begin : " << std::endl;
		std::cout << "fps : " << _fps << std::endl;
		std::cout << "Turn : " << _turns << std::endl;
		std::string playerTeamStr;

		if (_playerTeam == RED) {
			playerTeamStr = "RED";
		}

		if (_playerTeam == BLUE) {
			playerTeamStr = "BLUE";
		}

		std::cout << "PlayerTeam : " << playerTeamStr << std::endl;

		if (_board[_focusX + _focusY * _boardX]->holdsUnit()) {
			Character* character = _board[_focusX + _focusY * _boardX]->getCharacterPtr();
			std::cout << "Unit name : " << character->getName() << std::endl;
			switch (character->getTeam()) {
			default:
				std::cout << "default team" << std::endl;
				break;
			case BLUE:
				std::cout << "BLUE" << std::endl;
				break;
			case RED:
				std::cout << "RED" << std::endl;
				break;
			}
			std::cout << "Unit hp : " << character->getHp() << "/" << character->getMaxHp() << std::endl;
		}

		std::cout << "InfoTab Ends" << std::endl;
	}
}

void Game::nextTurn()
{
	_turns++;
	for (int i = 0; i < _entities.size(); i++) {
		_entities[i]->setUsed(false);
	}
	
	if (_playerTeam == RED) {
		_playerTeam = BLUE;
	}
	else if (_playerTeam == BLUE) {
		_playerTeam = RED;
	}
	_nTypesLeft[_playerTeam] = _nTypes[_playerTeam];

}

std::vector<Cell*> Game::getNeighbors(Cell * cell)
{
	int x = cell->getX();
	int y = cell->getY();
	std::vector<Cell*> neighbors;

	if (getCell(x + 1, y) != nullptr) {
		neighbors.push_back(getCell(x + 1, y));
	}
	if (getCell(x - 1, y) != nullptr) {
		neighbors.push_back(getCell(x - 1, y));
	}
	if (getCell(x, y + 1) != nullptr) {
		neighbors.push_back(getCell(x, y + 1));
	}
	if (getCell(x, y - 1) != nullptr) {
		neighbors.push_back(getCell(x, y - 1));
	}

	return neighbors;
}

void Game::highlightCells()
{
	for (int i = 0; i < _board.size(); i++) {
		if (_reachMap[_board[i]]) {
			_board[i]->setReachable(true);
		}
	}
}

void Game::clearCells()
{
	for (int i = 0; i < _board.size(); i++) {
		_board[i]->setReachable(false);
	}
}


void Game::genReachMap(Cell* start, int reach, bool fly) {
	//le parametre start désigne la case de départ, autrement dit, la case ou est le personnage
	//reach est la portée du personnage, de déplacement ou d'attaque
	//fly est vrai si on "vole" au dessus des trous et ennemis
	//pour la portée d'attaque, on le définit donc comme vrai (une fleche passe au dessus des trous).
	//par contre pour la portée de déplacement, on la définit fausse

	//les cases actuellement en traitement
	std::queue<Cell*> frontier;

	//les cases qui seront en traitement dans l'iteration d'après
	std::queue<Cell*> newFrontier;
	//un tableau qui associe les cases au fait qu'elles soient accessibles ou non
	std::map<Cell*, bool> visited;
	//tableau qui renseigne les voisins de la case étudiée
	std::vector<Cell*> neighbors;

	//on commence avec la case de départ
	frontier.push(start);
	visited[start] = true;

	//on execute pour chaque unité de mouvement du personnage
	for (int i = 0; i < reach; i++) {
		// on remplit neighbors de tous les voisins de chaque élement de la frontiere
		while (!frontier.empty()) {
			std::vector<Cell*> neighborsTemp = getNeighbors(frontier.front());
			neighbors.insert(neighbors.end(), neighborsTemp.begin(), neighborsTemp.end());
			frontier.pop();
		}
		for (int i = 0; i < neighbors.size(); i++){
			if (visited[neighbors[i]] == false) {
				//on ne prend pas compte des obstables
				if (fly) {
					visited[neighbors[i]] = true;
					newFrontier.push(neighbors[i]);
				}
				//on prend compte des obstacles
				else if (neighbors[i]->isWalkable()) {
					if (!neighbors[i]->holdsUnit()) {
						visited[neighbors[i]] = true;
						newFrontier.push(neighbors[i]);
					}
					else if (neighbors[i]->getCharacterPtr()->getTeam() == _playerTeam) {
						visited[neighbors[i]] = true;
						newFrontier.push(neighbors[i]);
					}
				}
			}
		}
		frontier = newFrontier;
	}
	_reachMap = visited;
	//donc chaque case qui est visitée est accessible par le personnage ou son attaque
}

void Game::updateTime() {
	Game::_time = SDL_GetTicks();
	Game::_deltaTime = (_time - _lastFrame) / 1000;
	Game::_lastFrame = _time;
}

void Game::updateFps() {
	_fps = 1.0f / _deltaTime;
}

void Game::selectCharacter(Character* character) {
	_gameState = SELECTED;
	_selectedCharacter = character;
	_selectedCharacter->setSelected(true);
	genReachMap(_board[_focusX + _focusY * _boardX], character->getReach(), false);
	highlightCells();
}

void Game::unselectCharacter() {
	_gameState = FREE;
	_selectedCharacter->setSelected(false);
	_selectedCharacter = nullptr;
	clearReachMap();
	clearCells();
}