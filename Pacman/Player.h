#pragma once
// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

struct Player
{
	bool dead;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	
	int direction;
	int frame;
	int currentFrameTime;
	float speedMultiplier;
	int dash;
	int dashduration;
	int collected;
	bool dashing;
};