#pragma once
#include<Blit3D.h>

enum class ASize { SMALL, MEDIUM, LARGE };

class Shot
{
public:
	glm::vec2 velocity, position;
	Sprite *sprite = NULL;
	float timeToLive = 2.f; //shots live for 2 seconds
	void Draw();
	bool Update(float seconds); //return false if shot dead (timeToLive <= 0)
};

class Ship
{
public:
	Sprite *shotSprite;
	std::vector<Sprite *> spriteList;
	glm::vec2 velocity, position;
	float angle = 0;
	float shotTimer = 0;
	float radius = 27.f;
	int lives = 3;
	int frameNumber = 0;
	float thrustTimer = 0;
	bool thrusting = false;
	bool turningLeft = false;
	bool turningRight = false;
	float noBoomTimer = 0;
	float boomTimer = 0;

	void Draw();
	void Update(float seconds);
	bool Shoot(std::vector<Shot> &shotList);
};

class Asteroid
{
public:
	glm::vec2 velocity, position;
	ASize aSize = ASize::LARGE;
	float radius;

	void Draw();
	void Update(float seconds);
};


bool CollisionWithShot(Asteroid& A, Shot& S);

bool CollisionWithShip(Asteroid& A, Ship& S);

void InitPerturb();
glm::vec2 Perturb(glm::vec2 V);
