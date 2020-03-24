#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

// Just need to include main header file
#include "S2D/S2D.h"
#include "Player.h"
#include "Enemy.h"
#include <iostream>
#define GHOSTCOUNT 12
#define MUNCHIECOUNT 50

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;


// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:
	// Data to represent Pacman
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;
	Player* _pacman;
	bool _sKeyDown;
	int score;
	// Data to represent Ghost
	MovingEnemy* _ghosts[GHOSTCOUNT];

	// Data to represent Munchie
	const int _cMunchieFrameTime;
	Enemy* _munchies[MUNCHIECOUNT];
	// Data to represent Cherry
	const int _cCherryFrameTime;
	Enemy* _cherry;
	
	// Position for String
	Vector2* _stringPosition;

	// Data for Menu
	Texture2D* _menuBackground;
	Rect* _menuRectangle;
	Vector2* _menuStringPosition;
	bool _paused;
	bool _startUp;
	bool _pKeyDown;
	
	SoundEffect* _pop;
	SoundEffect* _explode;

	void Pacman::Input(int elapsedTime, Input::KeyboardState* state)
	{
		// Checks if D,A,S,W key is pressed
		if (state->IsKeyDown(Input::Keys::D))
		{
			_pacman->position->X += _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier; //Moves Pacman across X axis
			_pacman->direction = 0;
		}
		else if (state->IsKeyDown(Input::Keys::A))
		{
			_pacman->position->X -= _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier; //Moves Pacman across X axis
			_pacman->direction = 2;
		}
		else if (state->IsKeyDown(Input::Keys::S))
		{
			_pacman->position->Y += _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier; //Moves Pacman across Y axis
			_pacman->direction = 1;
		}
		else if (state->IsKeyDown(Input::Keys::W))
		{
			_pacman->position->Y -= _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier; //Moves Pacman across Y axis
			_pacman->direction = 3;
		}
	}
	void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey)
	{
		if (state->IsKeyDown(startKey) && _startUp == true)
		{
			_startUp = false;
			_pacman->dash = 3;
		}
	}
	void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
	{
		if (state->IsKeyDown(pauseKey) && !_pKeyDown)
		{
			_pKeyDown = true;
			_paused = !_paused;
		}
		if (state->IsKeyUp(pauseKey))
			_pKeyDown = false;
	}
	void Pacman::SpeedMod(Input::KeyboardState* state, Input::Keys sprintKey)
	{
		if (state->IsKeyDown(sprintKey))
		{
			if (!_sKeyDown && _pacman->dash > 0)
			{
				_sKeyDown = true;
				_pacman->dashing = true;
				for (int i = 0; i < (_pacman->dash*_pacman->dashduration); i++)
				{
					_pacman->speedMultiplier += 0.02f;
				}
				_pacman->dashing = false;
				_pacman->dash--;
			}
			else
			{
				if (_pacman->speedMultiplier > 1.0f)
					_pacman->speedMultiplier -= 0.04f;
			}
		}
		if (state->IsKeyUp(sprintKey))
		{
			_sKeyDown = false;
			if (_pacman->speedMultiplier > 1.0f)
				_pacman->speedMultiplier -= 0.04f;
		}
	}
	void Pacman::CheckViewportCollision()
	{
		if (_pacman->position->X + _pacman->sourceRect->Width > Graphics::GetViewportWidth() + 72)
			_pacman->position->X = -24; // Creates Horizontal wrapping
		else if (_pacman->position->X - _pacman->sourceRect->Width < -96)
			_pacman->position->X = Graphics::GetViewportWidth() + 24;
		if (_pacman->position->Y + _pacman->sourceRect->Height > Graphics::GetViewportHeight() + 72)
			_pacman->position->Y = -24; // Creates Vertical wrapping
		else if (_pacman->position->Y - _pacman->sourceRect->Height < -96)
			_pacman->position->Y = Graphics::GetViewportHeight() + 24;
	}
	void Pacman::UpdatePacman(int elapsedTime)
	{
		_pacman->currentFrameTime += elapsedTime;
		if (_pacman->currentFrameTime > _cPacmanFrameTime)
		{
			_pacman->frame++;
			if (_pacman->frame >= 8)
				_pacman->frame = 0;
			_pacman->currentFrameTime = 0;
			score -= 1;
		}
		_pacman->sourceRect->X = _pacman->sourceRect->Width * _pacman->frame;
		_pacman->sourceRect->Y = _pacman->sourceRect->Height * _pacman->direction;
	}
	void Pacman::CheckCollisions()
	{
		int bottom1 = _pacman->position->Y + _pacman->sourceRect->Height, bottom2 = 0;
		int left1 = _pacman->position->X, left2 = 0;
		int right1 = _pacman->position->X + _pacman->sourceRect->Width, right2 = 0;
		int top1 = _pacman->position->Y, top2 = 0;
		for (int i = 0; i < GHOSTCOUNT; i++)
		{
			bottom2 = _ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
			left2 = _ghosts[i]->position->X;
			right2 = _ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
			top2 = _ghosts[i]->position->Y;
			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
			{
				Audio::Play(_explode);
				_pacman->dead = true;
				i = GHOSTCOUNT;
			}
		}
		for (int i = 0; i < MUNCHIECOUNT; i++)
		{
			bottom2 = _munchies[i]->Position->Y + _munchies[i]->SourceRect->Height;
			left2 = _munchies[i]->Position->X;
			right2 = _munchies[i]->Position->X + _munchies[i]->SourceRect->Width;
			top2 = _munchies[i]->Position->Y;
			if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
			{
				_munchies[i]->Position->Y = -100;
				_munchies[i]->Position->X = -100;
				Audio::Play(_pop);
				score += 1000;
				_pacman->collected++;
			}
		}
		bottom2 = _cherry->Position->Y + _cherry->SourceRect->Height;
		left2 = _cherry->Position->X;
		right2 = _cherry->Position->X + _cherry->SourceRect->Width;
		top2 = _cherry->Position->Y;
		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_cherry->Position->Y = rand() % Graphics::GetViewportHeight();
			_cherry->Position->X = rand() % Graphics::GetViewportWidth();
			Audio::Play(_pop);
			score += 2000;
			_pacman->dash++;
			_pacman->collected++;
			_cherry->collected++;
		}

	}
	void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
	{
		if (ghost->direction == 0)
			ghost->position->X += ghost->speed * elapsedTime;
		else if (ghost->direction == 2)
			ghost->position->X -= ghost->speed * elapsedTime;
		else if (ghost->direction == 1)
			ghost->position->Y += ghost->speed * elapsedTime;
		else if (ghost->direction == 3)
			ghost->position->Y -= ghost->speed * elapsedTime;
		if (ghost->position->X + ghost->sourceRect->Width >= Graphics::GetViewportWidth())
			ghost->direction = 2;
		else if (ghost->position->X <= 0)
			ghost->direction = 0;
		if (ghost->position->Y + ghost->sourceRect->Height >= Graphics::GetViewportHeight())
			ghost->direction = 3;
		else if (ghost->position->Y <= 0)
			ghost->direction = 1;
		ghost->sourceRect->Y = ghost->sourceRect->Height * ghost->direction;

	}
	void Pacman::UpdateMunchie(Enemy* munchie, int elapsedTime)
	{
		munchie->CurrentFrameTime += elapsedTime;
		if (munchie->CurrentFrameTime > _cMunchieFrameTime)
		{
			munchie->FrameCount++;
			if (munchie->FrameCount >= 2)
				munchie->FrameCount = 0;
			munchie->CurrentFrameTime = 0;
		}
	}
	void Pacman::UpdateCherry(Enemy* cherry, int elapsedTime)
	{
		cherry->CurrentFrameTime += elapsedTime;
		if (cherry->CurrentFrameTime > _cCherryFrameTime)
		{
			cherry->FrameCount++;
			if (cherry->FrameCount >= 2)
				cherry->FrameCount = 0;
			cherry->CurrentFrameTime = 0;
		}
	}
public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};