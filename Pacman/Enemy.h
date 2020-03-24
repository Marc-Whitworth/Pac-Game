#pragma once
// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

struct Enemy
{
	Vector2* Position;
	Rect* SourceRect;
	Texture2D* Texture;
	int collected;
	int CurrentFrameTime;
	int FrameCount;
	//int FrameTime;
};

struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};