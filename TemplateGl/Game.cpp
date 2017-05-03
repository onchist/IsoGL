#include "Game.h"

using namespace std;

//constructeur, initialise les attributs
Game::Game() {
	_window = nullptr;
	_screenWidth = 1024;
	_screenHeight = 576;
	_gameState = GameState::PLAY;
	_deltaTime = 0;
	_lastFrame = 0;
	_isoCamera = isoCamera();
	_ptrCoord = new int[2]{ 0,0 };
	_lampPosition = glm::vec3(1.2f, 1.0f, 2.0f);
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
			_keyPressed = true;
			
				inputArray[evnt.key.keysym.sym] = true;
				
			
			break;
		case SDL_KEYUP:
			_keyPressed = false;
			
				inputArray[evnt.key.keysym.sym] = false;
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
	
	for (int i = 0; i < _entities.size(); i++) {
		_entities[i]->draw();
	}

	SDL_GL_SwapWindow(_window);
}

void Game::update() {
	_time = SDL_GetTicks();
	_deltaTime = (_time - _lastFrame) / 1000;
	_lastFrame = _time;
	_fps = 1.0f / _deltaTime;

	if (inputArray[SDLK_ESCAPE]) {
		_gameState = GameState::EXIT;
	}
	_lampPosition.x = sin(_time/1000);
	if (inputArray[SDLK_z]) {
		_isoCamera.move(glm::vec3(0.0f, _deltaTime * 3.0f , 0.0f));
		/*if (!_firstInput[SDLK_z]) { 
			if (_ptrCoord[1] + 1 < _boardY) {
				_ptrCoord[1]++; _firstInput[SDLK_z] = true;
			}
		}*/
	}
	if (inputArray[SDLK_s]) {
		_isoCamera.move(glm::vec3(0.0f, -_deltaTime * 3.0f, 0.0f));
		/*if (!_firstInput[SDLK_s]) {
			if (_ptrCoord[1] - 1 >= 0) {
				_ptrCoord[1]--; _firstInput[SDLK_s] = true;
			}
		}*/
	}
	if (inputArray[SDLK_d]) {
		_isoCamera.move(glm::vec3(_deltaTime * 3.0f, 0.0f, 0.0f));
		/*if (!_firstInput[SDLK_d]) {
			if (_ptrCoord[0] + 1 < _boardX) {
				_ptrCoord[0]++; _firstInput[SDLK_d] = true;
			}
		}*/
	}
	if (inputArray[SDLK_q]) {
		_isoCamera.move(glm::vec3(-_deltaTime * 3.0f, 0.0f, 0.0f));
		/*if (!_firstInput[SDLK_q]) {
			if (_ptrCoord[0] - 1 >= 0) {
				_ptrCoord[0]--; _firstInput[SDLK_q] = true;
			}
		}*/
	}

	if (inputArray[SDLK_a]) {
		_isoCamera.rotate(_deltaTime * 1.0f);
	}
	if (inputArray[SDLK_e]) {
		_isoCamera.rotate(_deltaTime * -1.0f);
	}
	
	//_isoCamera.setTarget(_board[_ptrCoord[0]][_ptrCoord[1]]->getPos());
	//_isoCamera.setTarget(glm::vec3(_ptrCoord[0], _ptrCoord[1], 0.0f));
}


void Game::loadBoard() {
	
	/*_boardX = 3;
	_boardY= 3;

	float unit = 1.2f;
	_board = new Entity**[_boardX];

	for (int i = 0; i < _boardX; i++) {
		_board[i] = new Entity*[_boardY];
	}

	_board[0][0] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(0.0f, 0.0f, 0.0f));
	_board[1][0] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(unit, 0.0f, 0.0f));
	_board[2][0] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(2 * unit, 0.0f, 0.0f));

	_board[0][1] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(0.0f, unit, 0.0f));
	_board[1][1] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(unit, unit, 0.0f));
	_board[2][1] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(2 * unit, unit, 0.0f));

	_board[0][2] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(0.0f, 2 * unit, 0.0f));
	_board[1][2] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(unit, 2 * unit, 0.0f));
	_board[2][2] = new Entity(_program, _diffuseMap, _specularMap, _vaoCube, glm::vec3(2 * unit, 2 * unit, 0.0f));*/


	

	glm::vec3 light = glm::vec3(1.0f, 1.0f, 1.0f);
	LightMaterial material;
	material.ambient = light * 0.2f;
	material.diffuse = light * 0.7f;
	material.specular = light * 1.0f;

	glm::vec3 position = glm::vec3(-3.0f, -3.0f, 5.0f);

	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	PointLight* pointLight = new PointLight(material, position, constant, linear, quadratic);

	_lights.push_back(pointLight);

	ourModel = new Model("models/nanosuit/nanosuit.obj");
	_entities.push_back(new Character(_program, glm::vec3(0.0f),ourModel));

	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(-1.0f,-1.0f, 0.0f)));
	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(-1.0f,0.0f, 0.0f)));
	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(1.0f, 1.0f, 0.0f)));
	
	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(0.0f, -1.0f, 0.0f)));
	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(0.0f, 0.0f, 0.0f)));
	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(0.0f, 1.0f, 1.0f)));

	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(1.0f, -1.0f, 0.0f)));
	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(1.0f, 0.0f, 0.0f)));
	_entities.push_back(new Cell(_program, _diffuseMap, _specularMap, glm::vec3(1.0f, 1.0f, 0.0f)));
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


