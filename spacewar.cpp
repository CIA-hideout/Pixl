// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Draw animated spaceships with collision and shield
// Chapter 6 spacewar.cpp v1.0
// This class is the core of the game

#include "spaceWar.h"
#include <stdlib.h>
#include <stdio.h>
#include <timeapi.h>

double calculateF(Entity* e1, Entity* e2);

float			playerAcceleratioRate;
float			playerDeccelerationRate;
float			playerTurnMultiplier;

int				playerMaxHealth, playerHealth;
int				playerScore, playerLevel;
int				combo;

int				waveTriangleCount, waveCircleCount, waveBossCount;
int				currentWave;

bool			controlsInverted, blackholeRunning, isStunned, isEnlarged;

DWORD			waveStartTime, nextWaveTime;

//=============================================================================
// Constructor
//=============================================================================
Spacewar::Spacewar()
{}

//=============================================================================
// Destructor
//=============================================================================
Spacewar::~Spacewar()
{
	releaseAll();           // call onLostDevice() for every graphics item
}

//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void Spacewar::initialize(HWND hwnd) {

	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	int side;

	playerAcceleratioRate = 3.0f;
	playerDeccelerationRate = 0.005f;
	playerTurnMultiplier = 3.0f;

	playerHealth = 3;
	playerMaxHealth = 10;

	srand(timeGetTime());

	Game::initialize(hwnd); // throws GameError

	// main game textures
	shipTextures.initialize(graphics, PLAYER_TEXTURE);
	triangleTextures.initialize(graphics, TRIANGLE_TEXTURE);
	blackHoleTexture.initialize(graphics, BLACKHOLE_TEXTURE);

	player = new Ship();

	player->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
	player->setFrames(shipNS::player_START_FRAME, shipNS::player_END_FRAME);
	player->setCurrentFrame(shipNS::player_START_FRAME);
	player->setX(GAME_WIDTH / 4);
	player->setY(GAME_HEIGHT / 4);
	player->setVelocity(shipNS::SPEED, -shipNS::SPEED);
	player->setObjectType(PLAYER_SPRITE);
	player->setRadians(0);

	addEntity(player);

	blackhole = new Blackhole();
	blackhole->initialize(this, blackholeNS::WIDTH, blackholeNS::HEIGHT, blackholeNS::TEXTURE_COLS, &blackHoleTexture);
	blackhole->setFrames(blackholeNS::BLACKHOLE_START_FRAME, blackholeNS::BLACKHOLE_END_FRAME);
	blackhole->setCurrentFrame(blackholeNS::BLACKHOLE_START_FRAME);
	blackhole->setX(GAME_WIDTH / 2 - blackhole->getWidth() / 2 * blackhole->getScale());
	blackhole->setY(GAME_HEIGHT / 2 - blackhole->getHeight() / 2 * blackhole->getScale());
	blackhole->setVelocity(0, 0);
	blackhole->setObjectType(BLACKHOLE);
	blackhole->setRadians(0);
	blackhole->setScale(0.5f);

	addEntity(blackhole);

	//for (int i = 0; i < 10; i++) {

	//	Triangle* triangle = new Triangle();
	//	triangle->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
	//	triangle->setFrames(TriangleNS::TRIANGLE_START_FRAME, TriangleNS::TRIANGLE_END_FRAME);
	//	triangle->setCurrentFrame(TriangleNS::TRIANGLE_START_FRAME);
	//	triangle->setVelocity(10, 20);
	//	triangle->setObjectType(TRIANGLES);

	//	// random spawn on border
	//	side = rand() % 4;

	//	switch (side) {
	//		// left
	//		case 0: {
	//					triangle->setX(0 - rand() % GAME_WIDTH);
	//					triangle->setY(rand() % GAME_HEIGHT);
	//		} break;
	//			// top
	//		case 1: {
	//					triangle->setX(rand() % GAME_WIDTH);
	//					triangle->setY(-(rand() % GAME_HEIGHT));
	//		} break;
	//			// right
	//		case 2: {
	//					triangle->setX(GAME_WIDTH + GAME_WIDTH - rand() % GAME_WIDTH);
	//					triangle->setY(rand() % GAME_HEIGHT);
	//		} break;
	//			// bottom
	//		case 3: {
	//					triangle->setX(rand() % GAME_WIDTH);
	//					triangle->setY(GAME_HEIGHT + GAME_HEIGHT - rand() % GAME_HEIGHT);
	//		} break;
	//	}

	//	triangle->setX(rand() % GAME_WIDTH);
	//	triangle->setY(rand() % GAME_HEIGHT);

	//	triangle->setScale(0.5);

	//	addEntity(triangle);
	//}

	return;
}

//=============================================================================
// Update all game items
//=============================================================================
void Spacewar::update() {
	if (input->isKeyDown(0x42))
			blackhole->setMass(1);
	else
		blackhole->setMass(9999999999999.0f);
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void Spacewar::ai() {

}

//=============================================================================
// Render game items
//=============================================================================
void Spacewar::render()
{
	graphics->spriteBegin();                // begin drawing sprites

	DrawEntities();

	graphics->spriteEnd();                  // end drawing sprites
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void Spacewar::releaseAll()
{
	shipTextures.onResetDevice();
	triangleTextures.onResetDevice();
	Game::releaseAll();
	FreeConsole();
	return;
}

//=============================================================================
// The grahics device has been reset.
// Recreate all surfaces.
//=============================================================================
void Spacewar::resetAll()
{
	shipTextures.onResetDevice();
	triangleTextures.onResetDevice();
	Game::resetAll();
	return;
}

void Spacewar::addEntity(Entity* entity) {
	static int id_counter = 0;
	entity->setObjectId(id_counter);
	this->entities.push_back(entity);
	id_counter++;
}

void Spacewar::UpdateEntities() {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++){
		(*iter)->update(deltaTime);

		switch ((*iter)->getObjectType())
		{

			case PLAYER_SPRITE: {

									if (input->isKeyDown(VK_UP)) {
										(*iter)->setVelocity(
											(cos((*iter)->getRadians()) * playerAcceleratioRate + (*iter)->getVelocity().x),
											(sin((*iter)->getRadians()) * playerAcceleratioRate + (*iter)->getVelocity().y)
											);
									}

									if (input->isKeyDown(VK_LEFT)) {
										(*iter)->setRadians((*iter)->getRadians() - deltaTime * playerTurnMultiplier);
									}

									if (input->isKeyDown(VK_RIGHT)) {
										(*iter)->setRadians((*iter)->getRadians() + deltaTime * playerTurnMultiplier);
									}

									if (input->isKeyDown(0x5A)) {
										(*iter)->setVelocity(0, 0);
									}

									(*iter)->setVelocity(
										(*iter)->getVelocity().x - (*iter)->getVelocity().x * playerDeccelerationRate,
										(*iter)->getVelocity().y - (*iter)->getVelocity().y * playerDeccelerationRate
										);

									// calculate black hole's attraction here
									calculateF(blackhole, player);

									(*iter)->update(deltaTime);

			} break;

			case TRIANGLES: {
								double dx, dy;

								dx = player->getX() - (*iter)->getX();
								dy = player->getY() - (*iter)->getY();

								// 1, 4 quad
								if (dx > 0)
									(*iter)->setRadians(atan(dy / dx));
								// 2, 3 quad
								else if (dx < 0)
									(*iter)->setRadians(PI + atan(dy / dx));

								(*iter)->setVelocity(
									cos((*iter)->getRadians()) * 100,
									sin((*iter)->getRadians()) * 100
									);
								(*iter)->update(deltaTime);
			} break;

			case BLACKHOLE: {
								(*iter)->setRadians(timeGetTime() / 600.0f);
								(*iter)->update(deltaTime);
			} break;
		}
	}
}

void Spacewar::DrawEntities() {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++){
		(*iter)->draw();
	}
}

int genScore(int combo) {
	return 2 + int(pow(combo, 1.5));
}

void Spacewar::collisions()
{
	VECTOR2 collisionVector;
	// if collision between ship and planet
	if (player->collidesWith(*blackhole, collisionVector))
	{
		// bounce off planet
		player->bounce(collisionVector, *blackhole);
		player->damage(PLANET);
	}

	// if collision between ships
	//if (ship1.collidesWith(ship2, collisionVector))
	//{
	//	// bounce off ship
	//	ship1.bounce(collisionVector, ship2);
	//	ship1.damage(SHIP);
	//	// change the direction of the collisionVector for ship2
	//	ship2.bounce(collisionVector*-1, ship1);
	//	ship2.damage(SHIP);
	//}
}

double calculateF(Entity* e1, Entity* e2) {
	double deltaX = e1->getX() + e1->getWidth() / 2 * e1->getScale() - e2->getX() - e2->getWidth() * e2->getScale() / 2;
	double deltaY = e1->getY() + e1->getHeight() / 2 * e1->getScale() - e2->getY() - e2->getHeight() * e2->getScale() / 2;
	double distance = sqrt(pow(deltaX, 2) + pow(deltaY, 2));
	// force
	double force = G * e2->getMass() * e1->getMass() / pow(distance, 2);
	// deltaV

	double A1 = force / e1->getMass();
	double A2 = force / e2->getMass();

	e1->setVelocity(
		e1->getVelocity().x + deltaX * A1,
		e1->getVelocity().y + deltaY * A1
		);

	e2->setVelocity(
		e2->getVelocity().x + deltaX * A2,
		e2->getVelocity().y + deltaY * A2
		);

	printf("%.10f, %.10f, %.10f\n", A1, A2, e1->getMass());

	return force;
}