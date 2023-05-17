/*
	Asteroids ship movement example
*/
//memory leak detection
#define CRTDBG_MAP_ALLOC
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif  // _DEBUG

#include <stdlib.h>
#include <crtdbg.h>

#include "Blit3D.h"

#include "Ship.h"

Blit3D *blit3D = NULL;

float baseVelocity = 200.f;

//GLOBAL DATA
double elapsedTime = 0;
float timeSlice = 1.f / 120.f;
int Score = 0;
int Lives = 3;
int deathCounter = 1;

Sprite* TitleSprite = NULL;
Sprite *backgroundSprite = NULL; //a pointer to a background sprite
Ship *ship = NULL;
Sprite* AsteroidSprites[3];
AngelcodeFont* aFont = NULL;



enum class GameMode{ PLAYING, STARTGAME, HISCORE, CREDITS };
GameMode gameMode = GameMode::STARTGAME;

bool shoot = false; //should we try to shoot?
std::vector<Shot> shotList;

std::vector<Asteroid> asteroidList;



void Init()
{
	//turn cursor off
	blit3D->ShowCursor(false);

	TitleSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\Title.png");
	//load our background image: the arguments are upper-left corner x, y, width to copy, height to copy, and file name.
	backgroundSprite = blit3D->MakeSprite(0, 0, 1920, 1080, "Media\\SpaceBackground.png");

	//create a ship
	ship = new Ship;
	//load a sprite off of a spritesheet
	for (int i = 0; i < 4; ++i)
		ship->spriteList.push_back(blit3D->MakeSprite(i * 45, 0, 45, 85, "Media\\ShipSheet.png"));

	ship->position = glm::vec2(1920.f / 2, 1080.f / 2);



	//load the shot graphic
	ship->shotSprite = blit3D->MakeSprite(0, 0, 8, 8, "Media\\shot.png");

	AsteroidSprites[(int)ASize::LARGE] = blit3D->MakeSprite(0, 0, 241, 212, "Media\\LargeAsteroid.png");
	AsteroidSprites[(int)ASize::MEDIUM] = blit3D->MakeSprite(0, 0, 234, 154, "Media\\MediumAsteroid.png");
	AsteroidSprites[(int)ASize::SMALL] = blit3D->MakeSprite(0, 0, 139, 135, "Media\\SmallAsteroid.png");

	aFont = blit3D->MakeAngelcodeFontFromBinary32("Media\\PlanetTaryCommandFont75.bin");

	Asteroid a;

	a.position = glm::vec2(1920, 1080);
	a.velocity = glm::vec2(350, 400);
	a.aSize = ASize::LARGE;
	a.radius = 120;
	asteroidList.push_back(a);

	Asteroid b;

	b.position = glm::vec2(190, 600);
	b.velocity = glm::vec2(350, 400);
	b.aSize = ASize::MEDIUM;
	b.radius = 117;
	asteroidList.push_back(b);

	Asteroid c;

	c.position = glm::vec2(600, 0);
	c.velocity = glm::vec2(250, 150);
	c.aSize = ASize::SMALL;
	c.radius = 69;
	asteroidList.push_back(c);
	//set the clear colour
	glClearColor(1.0f, 0.0f, 1.0f, 0.0f);	//clear colour: r,g,b,a 	
}

void DeInit(void)
{
	if(ship != NULL) delete ship;
	//any sprites/fonts still allocated are freed automatically by the Blit3D object when we destroy it
}

void Update(double seconds)
{
	//only update time to a maximun amount - prevents big jumps in 
	//the simulation if the computer "hiccups"
	if (seconds < 0.15)
		elapsedTime += seconds;
	else elapsedTime += 0.15;

	switch (gameMode)
	{
	case GameMode::PLAYING:
		//update by a full timeslice when it's time
		while (elapsedTime >= timeSlice)
		{
			elapsedTime -= timeSlice;
			ship->Update(timeSlice);

			if (shoot) ship->Shoot(shotList);

			//iterate backwards through the shotlist,
			//so we can erase shots without messing up the vector
			//for the next loop
			for (int i = shotList.size() - 1; i >= 0; --i)
			{
				//shot Update() returns false when the bullet should be killed off
				if (!shotList[i].Update(timeSlice))
					shotList.erase(shotList.begin() + i);
			}
			for (auto& a : asteroidList)
			{
				a.Update(timeSlice);
			}
			if (asteroidList.empty())
			{
				Asteroid a;

				a.position = glm::vec2(1920, 1080);
				a.velocity = glm::vec2(350, 400);
				a.aSize = ASize::LARGE;
				a.radius = 120;
				asteroidList.push_back(a);

				Asteroid b;

				b.position = glm::vec2(190, 600);
				b.velocity = glm::vec2(350, 400);
				b.aSize = ASize::MEDIUM;
				b.radius = 117;
				asteroidList.push_back(b);

				Asteroid c;

				c.position = glm::vec2(600, 0);
				c.velocity = glm::vec2(250, 150);
				c.aSize = ASize::SMALL;
				c.radius = 69;
				asteroidList.push_back(c);
			}

			for (int i = asteroidList.size() - 1; i >= 0; --i)
			{
				for (int j = shotList.size() - 1; j >= 0; --j)
				{
					if (CollisionWithShot(asteroidList[i], shotList[j]))
					{
						//erase the shot
						shotList.erase(shotList.begin() + j);

						switch (asteroidList[i].aSize)
						{
						case ASize::SMALL:
							asteroidList.erase(asteroidList.begin() + i);
							Score += 30;
							break;

						case ASize::LARGE:
						{
							Score += 10;
							//make a new asteroid
							Asteroid a;
							a.aSize = ASize::MEDIUM;
							a.position = asteroidList[i].position;
							a.radius = 117.f;
							a.velocity = Perturb(asteroidList[i].velocity);
							a.velocity = glm::normalize(a.velocity) * baseVelocity * 1.5f;

							//set A.angle and rotationSpeed randomly
							asteroidList.push_back(a);


							//change the old asteroid to be smaller
							asteroidList[i].aSize = ASize::MEDIUM;
							asteroidList[i].radius = 117.f;
							asteroidList[i].velocity = glm::normalize(asteroidList[i].velocity) * baseVelocity * 1.5f;
						}
						break;

						case ASize::MEDIUM:
						{
							Score += 20;
							//make a new asteroid
							Asteroid b;
							b.aSize = ASize::SMALL;
							b.position = asteroidList[i].position;
							b.radius = 69.f;
							b.velocity = Perturb(asteroidList[i].velocity);
							b.velocity = glm::normalize(b.velocity) * baseVelocity * 1.5f;

							//set A.angle and rotationSpeed randomly
							asteroidList.push_back(b);


							//change the old asteroid to be smaller
							asteroidList[i].aSize = ASize::SMALL;
							asteroidList[i].radius = 69.f;
							asteroidList[i].velocity = glm::normalize(asteroidList[i].velocity) * baseVelocity * 3.f;
						}
						}
						break;
					}
				
				}
			}
			for (int i = asteroidList.size() - 1; i >= 0; --i)
			{
				if (CollisionWithShip(asteroidList[i], *ship))
				{
					if (Lives > 0)
					{
						Lives--;
						ship->noBoomTimer = 3;
					}
				}
				if (deathCounter == 1)
				{
					if (Lives == 0)
					{
						ship->boomTimer = 4;
						deathCounter--;
					}
				}

			}
			if (ship->boomTimer < 0)
			{
				gameMode = GameMode::HISCORE;
			}
		}
		break;
	case GameMode::HISCORE:
		break;
	case GameMode::STARTGAME:
		shotList.clear();
		ship->position = glm::vec2(1920 / 2, 1080 / 2);
		ship->velocity = glm::vec2(0, 0);
		ship->angle = -90;
		break;
	}
}


void Draw(void)
{
	// wipe the drawing surface clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw stuff here
	switch (gameMode)
	{
	case GameMode::PLAYING:
		//draw the background in the middle of the screen
		//the arguments to Blit(0 are the x, y pixel coords to draw the center of the sprite at, 
		//starting as 0,0 in the bottom-left corner.
		backgroundSprite->Blit(1920.f / 2, 1080.f / 2);


		//draw the shots
		for (auto& S : shotList) S.Draw();

		//draw the ship
		ship->Draw();

		//draw the asteroid
		for (auto& S : asteroidList) S.Draw();

		//draw text
		aFont->BlitText(20.f, 1032.f, "Score: " + std::to_string(Score));
		aFont->BlitText(1450.f, 1032.f, "Lives: " + std::to_string(Lives));
		break;
	case GameMode::HISCORE:
		backgroundSprite->Blit(1920.f / 2, 1080.f / 2);
		aFont->BlitText(20.f, 100.f, "Your Score is " + std::to_string(Score));
		break;
	case GameMode::CREDITS:
		backgroundSprite->Blit(1920.f / 2, 1080.f / 2);
		aFont->BlitText(100.f, 1080.f / 2, "Thanks for playing!! Game by Jared Shaddick.");
		break;
	case GameMode::STARTGAME:
		TitleSprite->Blit(1920 / 2, 1080 / 2);
	}
}

//the key codes/actions/mods for DoInput are from GLFW: check its documentation for their values
void DoInput(int key, int scancode, int action, int mods)
{
	switch (gameMode)
	{

	case GameMode::PLAYING:
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			blit3D->Quit(); //start the shutdown sequence

		if (key == GLFW_KEY_A && action == GLFW_PRESS)
			ship->turningLeft = true;

		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
			ship->turningLeft = false;

		if (key == GLFW_KEY_D && action == GLFW_PRESS)
			ship->turningRight = true;

		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
			ship->turningRight = false;

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
			ship->thrusting = true;

		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
			ship->thrusting = false;

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
			shoot = true;

		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
			shoot = false;
		break;
	case GameMode::STARTGAME:
		gameMode = GameMode::PLAYING;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			blit3D->Quit(); //start the shutdown sequence
		break;
	case GameMode::CREDITS:
		gameMode = GameMode::STARTGAME;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			blit3D->Quit(); //start the shutdown sequence
		break;
	case GameMode::HISCORE:
		gameMode = GameMode::CREDITS;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			blit3D->Quit(); //start the shutdown sequence
	}

}

int main(int argc, char *argv[])
{
	//memory leak detection
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//set X to the memory allocation number in order to force a break on the allocation:
	//useful for debugging memory leaks, as long as your memory allocations are deterministic.
	//_crtBreakAlloc = X;

	blit3D = new Blit3D(Blit3DWindowModel::BORDERLESSFULLSCREEN_1080P, 640, 400);

	//set our callback funcs
	blit3D->SetInit(Init);
	blit3D->SetDeInit(DeInit);
	blit3D->SetUpdate(Update);
	blit3D->SetDraw(Draw);
	blit3D->SetDoInput(DoInput);

	//Run() blocks until the window is closed
	blit3D->Run(Blit3DThreadModel::SINGLETHREADED);
	if (blit3D) delete blit3D;
}

