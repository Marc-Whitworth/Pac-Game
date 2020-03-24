	#include "Pacman.h"
#include <iostream>
#include <sstream>
#

Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(50), _cMunchieFrameTime(500), _cCherryFrameTime(100)
{
	_pacman = new Player();
	_pacman->dead = false;
	_paused = false;
	_pKeyDown = false;
	_startUp = true;
	_pacman->direction = 0;
	_pacman->currentFrameTime = 0;
	_pacman->frame = 0;
	_pacman->speedMultiplier = 1;
	_pacman->dashduration = 75;
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = rand() % 3;
		_ghosts[i]->speed = 0.2f;
	}
	//Munchie initialise
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->CurrentFrameTime = 0;
		_munchies[i]->FrameCount = rand() % 1;
	}
	//Cherry initialise
	_cherry = new Enemy();
	_cherry->CurrentFrameTime = 0;
	_cherry->FrameCount = rand() % 1;
	_cherry->collected = 0;
	_pop = new SoundEffect();
	_explode = new SoundEffect();

	//Initialise important Game aspects
	Audio::Initialise();
	if (!Audio::IsInitialised())
	{
		std::cout << " audio not init" << std::endl;
	}
	Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
	Input::Initialise();

	// Start the Game Loop - This calls Update and Draw in game loop
	Graphics::StartGameLoop();
}

Pacman::~Pacman()
{
	for (int nCount = 0; nCount < MUNCHIECOUNT; nCount++)
	{
		delete _munchies[nCount]->SourceRect;
		delete _munchies[nCount]->Position;
		delete _munchies[nCount];
	}
	delete[] _munchies;
	delete _pacman->texture;
	delete _munchies[0]->Texture;	
	delete _ghosts[0]->texture;
	delete _pacman->sourceRect;
	delete _pacman;
	delete _pop;
	delete _explode;
	for (int nCount = 0; nCount < GHOSTCOUNT; nCount++)
	{
		delete _ghosts[nCount]->sourceRect;
		delete _ghosts[nCount]->position;
		delete _ghosts[nCount];
	}
	delete[] _ghosts;
	delete _cherry->Texture;
	delete _cherry->Position;
	delete _cherry->SourceRect;
	delete _cherry;

}

void Pacman::LoadContent()
{
	_pop->Load("Sounds/pop.wav");
	if (!_pop->IsLoaded())
	{
		std::cout << "Pop was not loaded!" << std::endl;
	}
	_explode->Load("Sounds/Explode.wav");
	// Load Pacman
	_pacman->texture = new Texture2D();
	_pacman->texture->Load("Textures/Sonic.png", false);
	_pacman->position = new Vector2(350.0f, 350.0f);
	_pacman->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Load Munchie
	Texture2D* munchieTex = new Texture2D();
	munchieTex->Load("Textures/munchie.png", false);
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->Texture = munchieTex;
		_munchies[i]->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_munchies[i]->SourceRect = new Rect(0.0f, 0.0f, 12, 12 );
	}
	Texture2D* ghostTex = new Texture2D();
	ghostTex->Load("Textures/Ghost.png", false);
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->texture = ghostTex;
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
		_ghosts[i]->sourceRect->X = _ghosts[i]->sourceRect->Width * (i % 4);
	}
	Texture2D* cherryTex = new Texture2D();
	cherryTex->Load("Textures/cherry.png", false);
	_cherry->Texture = cherryTex;
	_cherry->Position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
	_cherry->SourceRect = new Rect(0.0f, 0.0f, 32, 32);
	

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set Menu Parameters
	_menuBackground = new Texture2D();
	_menuBackground->Load("Textures/Transparency.png", false);
	_menuRectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_menuStringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);
	
}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	// Start Menu
	CheckStart(keyboardState, Input::Keys::SPACE);
	
	if (!_startUp)
	{
		CheckPaused(keyboardState, Input::Keys::P);
		if (!_paused)
		{
			Input(elapsedTime, keyboardState);
			SpeedMod(keyboardState, Input::Keys::LEFTSHIFT);
			CheckViewportCollision();
			UpdatePacman(elapsedTime);
			for (int i = 0; i < GHOSTCOUNT; i++)
				UpdateGhost(_ghosts[i], elapsedTime);
				CheckCollisions();
			for (int i = 0; i < MUNCHIECOUNT; i++)
				UpdateMunchie(_munchies[i],elapsedTime);
			UpdateCherry(_cherry, elapsedTime);
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Score " << score << " Dashes " << _pacman->dash;

	SpriteBatch::BeginDraw(); // Starts Drawing
	if (_pacman->dead == false)
		SpriteBatch::Draw(_pacman->texture, _pacman->position, _pacman->sourceRect); // Draws Pacman
	else
		Graphics::Destroy();
	if (_pacman->collected > MUNCHIECOUNT)
	{
		cout << "Congratulations! Your score was: " << score;
		Graphics::Destroy();
	}
	

	for (int i = 0; i < GHOSTCOUNT; i++)
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);

	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(_munchies[i]->Texture, _munchies[i]->Position, _munchies[i]->SourceRect); //Draws Munchie
		if (_munchies[i]->FrameCount == 0)
		{
			// Draws Red Munchie
			_munchies[i]->SourceRect->X = _munchies[i]->SourceRect->Width * _munchies[i]->FrameCount;
		}
		else
		{
			// Draw Blue Munchie
			_munchies[i]->SourceRect->X = _munchies[i]->SourceRect->Width * _munchies[i]->FrameCount;
			if (_munchies[i]->FrameCount >= 60)
				_munchies[i]->FrameCount = 0;
		}
	}
	SpriteBatch::Draw(_cherry->Texture, _cherry->Position, _cherry->SourceRect); //Draws Munchie
	if (_cherry->FrameCount == 0)
	{
		// Draws Red Cherry
		_cherry->SourceRect->X = _cherry->SourceRect->Width * _cherry->FrameCount;
	}
	else
	{
		// Draw Blue Cherry
		_cherry->SourceRect->X = _cherry->SourceRect->Width * _cherry->FrameCount;
		if (_cherry->FrameCount >= 60)
			_cherry->FrameCount = 0;
	}
	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	
	if (_startUp)
	{
		std::stringstream menuStream;
		menuStream << "PRESS SPACE TO BEGIN";
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Cyan);
	}
	 else if (_paused)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";
		SpriteBatch::Draw(_menuBackground, _menuRectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _menuStringPosition, Color::Red);
	}
	SpriteBatch::EndDraw(); // Ends Drawing
}