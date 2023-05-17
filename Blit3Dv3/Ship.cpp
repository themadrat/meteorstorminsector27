#include "Ship.h"
#include<random>

std::mt19937 rng;
std::uniform_real_distribution<float> distPerturb(-0.25f, 0.25f);


extern Sprite* AsteroidSprites[3];

void Shot::Draw()
{
	sprite->Blit(position.x, position.y);
	//left
	sprite->Blit(position.x - 1920.f, position.y);
	//right
	sprite->Blit(position.x + 1920.f, position.y);
	//down
	sprite->Blit(position.x, position.y - 1080.f);
	//up
	sprite->Blit(position.x, position.y + 1080.f);
	//corners
	sprite->Blit(position.x + 1920.f, position.y + 1080.f);
	sprite->Blit(position.x - 1920.f, position.y - 1080.f);
	sprite->Blit(position.x - 1920.f, position.y + 1080.f);
	sprite->Blit(position.x + 1920.f, position.y - 1080.f);
}

bool Shot::Update(float seconds)
{
	//TODO:
	// determine if we are past the end of our life time,
	//if so return false
	timeToLive -= seconds;
	if (timeToLive <= 0)
	{
		return false;
	}
	//move the shot
	position += velocity * seconds;

	//TODO:
	//wrap around
	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	return true;
}

void Ship::Draw()
{
	if (boomTimer > 0)
	{
		return;
	}
	else
	{
		//change ship angle because my graphics face "up", not "right"
		spriteList[frameNumber]->angle = angle - 90;

		//draw main ship sprite
		spriteList[frameNumber]->Blit(position.x, position.y);

		//redraw if too close to an edge
		//left
		if (position.x < radius + 10.f) spriteList[frameNumber]->Blit(position.x + 1920.f, position.y);
		//right
		if (position.x > 1920.f - (radius + 10.f)) spriteList[frameNumber]->Blit(position.x - 1920.f, position.y);
		//down
		if (position.y < radius + 10.f) spriteList[frameNumber]->Blit(position.x, position.y + 1080.f);
		//up
		if (position.y > 1080.f - (radius + 10.f)) spriteList[frameNumber]->Blit(position.x, position.y - 1080.f);

		//copies for 4 diagonal corners
		spriteList[frameNumber]->Blit(position.x + 1920.f, position.y + 1080.f);
		spriteList[frameNumber]->Blit(position.x - 1920.f, position.y - 1080.f);
		spriteList[frameNumber]->Blit(position.x - 1920.f, position.y + 1080.f);
		spriteList[frameNumber]->Blit(position.x + 1920.f, position.y - 1080.f);
	}
}

void Ship::Update(float seconds)
{
	//handle turning
	if (turningLeft)
	{
		angle += 180.f * seconds;
	}

	if (turningRight)
	{
		angle -= 180.f * seconds;
	}

	if (thrusting)
	{
		//calculate facing vector
		float radians = angle * (M_PI / 180);
		glm::vec2 facing;
		facing.x = std::cos(radians);
		facing.y = std::sin(radians);

		facing *= seconds * 400.f;

		velocity += facing;

		//check if over max speed
		if (velocity.length() > 600.f)
		{
			velocity = glm::normalize(velocity) * 600.f;
		}

		thrustTimer += seconds;
		
		//animation timing
		if (thrustTimer >= 1.f / 20.f)
		{
			//change frames
			frameNumber++;
			if (frameNumber > 3)
				frameNumber = 1;

			thrustTimer -= 1.f / 20.f;
		}
	}
	else frameNumber = 0;

	//update position
	position += velocity * seconds;

	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;

	//reduce velocity due to "space friction"
	float scale = 1.f - seconds * 0.5f;
	if (scale < 0) scale = 0;
	velocity *= scale;

	//velocity could potentialy get very small:we should
	//cap it to zero when it gets really close,
	//but that is a more advanced topic that
	//requires techniques like std::isnan()
	float length = glm::length(velocity);
	if(std::isnan(length) || length < 0.00001f)
		velocity = glm::vec2(0, 0);

	//TODO:
	//handle shot timer
	if (shotTimer > 0)
	{
		shotTimer = shotTimer - seconds;
	}
	//handle invincible timer
	if (noBoomTimer > 0)
	{
		noBoomTimer -= seconds;
	}
	if (boomTimer > 0)
	{
		boomTimer -= seconds;
	}
	
}

bool Ship::Shoot(std::vector<Shot> &shotList)
{

 	if(shotTimer > 0) return false;

	//TODO:
	//time to shoot!
	//reset shot timer
	shotTimer = 0.2f;
	//make a new shot
	Shot Lazer;
	//set the shot's sprite and position using the ship's variables
	Lazer.position = position;
	Lazer.sprite = shotSprite;
	//build a vector from the ship angle
	float radians = angle * (M_PI / 180);
	glm::vec2 facing;
	facing.x = std::cos(radians);
	facing.y = std::sin(radians);

	velocity += facing;

	//scale up the shot velocity
	facing *= 600;

	//add the ship velocity 
	Lazer.velocity = velocity + facing;

	//add the shot to the shotList
	shotList.push_back(Lazer);

	return true;
}

void Asteroid::Draw()
{
	AsteroidSprites[(int) aSize]->Blit(position.x, position.y);

	//left
	AsteroidSprites[(int)aSize]->Blit(position.x - 1920.f, position.y);
	//right
	AsteroidSprites[(int)aSize]->Blit(position.x + 1920.f, position.y);
	//down
	AsteroidSprites[(int)aSize]->Blit(position.x, position.y - 1080.f);
	//up
	AsteroidSprites[(int)aSize]->Blit(position.x, position.y + 1080.f);
	//corners
	AsteroidSprites[(int)aSize]->Blit(position.x + 1920.f, position.y + 1080.f);
	AsteroidSprites[(int)aSize]->Blit(position.x - 1920.f, position.y - 1080.f);
	AsteroidSprites[(int)aSize]->Blit(position.x - 1920.f, position.y + 1080.f);
	AsteroidSprites[(int)aSize]->Blit(position.x + 1920.f, position.y - 1080.f);
}

void Asteroid::Update(float seconds)
{
	//move the rock
	position += velocity * seconds;
	//wrap around
	//bounds check position
	if (position.x < 0) position.x += 1920.f;
	if (position.x > 1920) position.x -= 1920.f;
	if (position.y < 0) position.y += 1080.f;
	if (position.y > 1080) position.y -= 1080.f;
}

bool CollisionWithShot (Asteroid &A, Shot &S)
{
	if (glm::length(A.position - S.position) <= A.radius)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CollisionWithShip(Asteroid& A, Ship& S)
{
	if (S.noBoomTimer > 0)
	{
		return false;
	}
	else if (glm::length(A.position - S.position) <= A.radius)
	{
		return true;
	}
	else
	{
		return false;
	}
}

glm::vec2 Perturb(glm::vec2 V)
{
	float length = glm::length(V);
	float angle = atan2f(V.y, V.x);
	angle += distPerturb(rng);

	V.x = cos(angle);
	V.y = sin(angle);

	V *= length;
	return V;
}

void InitPerturb()
{
	std::random_device rd;
	rng.seed(rd());
}
