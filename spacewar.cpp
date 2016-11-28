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
#include <sstream>
#include <iomanip>

double calculateF(Entity* e1, Entity* e2);

float			playerAcceleratioRate;
float			playerDeccelerationRate;
float			playerTurnMultiplier;
float			playerInvulnerableTimer;

int				playerMaxHealth, playerHealth;
int				playerScore, playerLevel;
int				combo;

int				waveTriangleCount, waveCircleCount, waveBossCount;
int				currentWave;

bool			controlsInverted, blackholeRunning, isStunned, isEnlarged;
bool			playerIsInvulnerable, playerIsDead;

DWORD			waveStartTime, nextWaveTime;

DWORD			baseTime, currTime;

std::stringstream ss;

//=============================================================================
// Constructor
//=============================================================================
Spacewar::Spacewar() {

}

//=============================================================================
// Destructor
//=============================================================================
Spacewar::~Spacewar() {
	releaseAll();           // call onLostDevice() for every graphics item
}

//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void Spacewar::initialize(HWND hwnd) {

	//AllocConsole();			// Brings up console

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	int side;

	playerAcceleratioRate = 3.5f;
	playerDeccelerationRate = 0.005f;
	playerTurnMultiplier = 3.5f;

	playerInvulnerableTimer = 2000.0f;
	playerIsInvulnerable = false;
	playerIsDead = false;

	playerHealth = 3;
	playerMaxHealth = 10;

	srand(timeGetTime());

	Game::initialize(hwnd); // throws GameError

	// main game textures
	shipTextures.initialize(graphics, PLAYER_TEXTURE);
	p_deathTextures.initialize(graphics, PLAYER_DEATH_TEXTURE);
	triangleTextures.initialize(graphics, TRIANGLE_TEXTURE);
	circleTextures.initialize(graphics, CIRCLE_TEXTURE);
	blackHoleTexture.initialize(graphics, BLACKHOLE_TEXTURE);
	heartTexture.initialize(graphics, HEART_TEXTURE);
	fontTexture.initialize(graphics, FONT_TEXTURE);

	timeFont = new Font();
	timeFont->initialize(this, 2048, 2048, 16, &fontTexture);
	timeFont->loadTextData(FONT_TEXTURE_INFO);
	timeFont->setHeight(128);
	timeFont->setWidth(128);

	comboFont = new Font();
	comboFont->initialize(this, 2048, 2048, 16, &fontTexture);
	comboFont->loadTextData(FONT_TEXTURE_INFO);
	comboFont->setHeight(128);
	comboFont->setWidth(128);

	scoreFont = new Font();
	scoreFont->initialize(this, 2048, 2048, 16, &fontTexture);
	scoreFont->loadTextData(FONT_TEXTURE_INFO);
	scoreFont->setHeight(128);
	scoreFont->setWidth(128);

	player = new Ship();
	player->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
	player->setFrames(shipNS::player_START_FRAME, shipNS::player_END_FRAME);		//<-
	player->setCurrentFrame(shipNS::player_START_FRAME);							//<- supposedly not needed right
	player->setX(GAME_WIDTH / 4);
	player->setY(GAME_HEIGHT / 4);
	player->setVelocity(shipNS::SPEED, -shipNS::SPEED);
	player->setObjectType(PLAYER_SPRITE);
	player->setRadians(0);
	player->setHealth(3);

	addEntity(player);
	blackhole = new Blackhole();
	blackhole->initialize(this, blackholeNS::WIDTH, blackholeNS::HEIGHT, blackholeNS::TEXTURE_COLS, &blackHoleTexture);
	blackhole->setFrames(blackholeNS::BLACKHOLE_START_FRAME, blackholeNS::BLACKHOLE_END_FRAME);
	blackhole->setCurrentFrame(blackholeNS::BLACKHOLE_START_FRAME);
	blackhole->setX(GAME_WIDTH / 2 - blackhole->getWidth() / 2 * blackhole->getScale());
	blackhole->setY(GAME_HEIGHT / 2 - blackhole->getHeight() / 2 * blackhole->getScale());
	blackhole->setVelocity(0, 0);
	blackhole->setObjectType(BLACKHOLE_);
	blackhole->setRadians(0);
	blackhole->setScale(0.5f);

	addEntity(blackhole);

	int dx = GAME_WIDTH - 10;

	for (int i = 0; i < 10; i++) {

		Entity* heart = new Entity();
		heart->initialize(this, 128, 128, 2, &heartTexture);
		heart->setCurrentFrame(0);
		heart->setVelocity(0, 0);
		heart->setMass(0);
		heart->setScale(0.5);

		heart->setX(dx - heart->getWidth() * heart->getScale());
		heart->setY(GAME_HEIGHT - heart->getHeight() * heart->getScale() - 10);

		dx -= heart->getWidth() * heart->getScale();

		hearts.push_back(heart);

	}

	// Spawn Triangles
	for (int i = 0; i < 0; i++) {

		Triangle* triangle = new Triangle();
		triangle->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
		triangle->setFrames(TriangleNS::TRIANGLE_START_FRAME, TriangleNS::TRIANGLE_END_FRAME);
		triangle->setCurrentFrame(TriangleNS::TRIANGLE_START_FRAME);
		triangle->setVelocity(10, 20);
		triangle->setObjectType(TRIANGLES);

		triangle->spawn();		//	spawn triangle in its own method.
		addEntity(triangle);
	}

	// Spawn Circles
	for (int i = 0; i < 10; i++){

		Circle* circle = new Circle();
		circle->initialize(this, CircleNS::WIDTH, CircleNS::HEIGHT, CircleNS::TEXTURE_COLS, &circleTextures);
		circle->setFrames(CircleNS::CIRCLE_START_FRAME, CircleNS::CIRCLE_END_FRAME);
		circle->setCollisionRadius((circle->getHeight()) / 2);
		circle->setVelocity(100, 200);
		circle->setObjectType(CIRCLES);
		circle->setScale(0.2f);

		circle->spawn();
		addEntity(circle);
	}

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

	baseTime = timeGetTime();

	return;
}

void Spacewar::update() {

	if (input->isKeyDown(0x42))
		blackhole->setMass(1);
	else
		blackhole->setMass(9999999999999.0f);

	if (player->getHealth() > 0) {
		for (int i = 0; i < hearts.size() - player->getHealth(); i++) {
			hearts[i]->setCurrentFrame(1);
		}
	}
	for (int i = player->getHealth(); i < hearts.size(); i++) {
		hearts[i]->setCurrentFrame(0);
	}

}

void Spacewar::ai() {

}

void Spacewar::render()
{
	graphics->spriteBegin();                // begin drawing sprites

	ss.str("");
	ss << std::fixed << std::setprecision(1) << (float)(timeGetTime() - baseTime) / 1000;
	timeFont->Print(GAME_WIDTH / 2 - timeFont->getTotalWidth(ss.str()) / 2, 10, ss.str());
	ss.str("x" + std::to_string(combo));
	comboFont->Print(10, GAME_HEIGHT - comboFont->getHeight() * comboFont->getScale(), ss.str());
	// ss.str(std::to_string(playerScore));
	ss.str(std::to_string(9993));
	scoreFont->Print(10, 10, ss.str());

	for (std::vector<Entity*>::iterator iter = hearts.begin(); iter != hearts.end(); iter++) {
		(*iter)->draw();
	}

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
	circleTextures.onResetDevice();
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
								if (!playerIsDead)
								{
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

									printf("%.2f, %d\n", player->getHealth(), playerIsInvulnerable);

									if (playerIsInvulnerable) {
										playerInvulnerableTimer -= deltaTime;
										if (playerInvulnerableTimer < 0) {
											playerIsInvulnerable = false;
										}
									}
								}
								else
								{
									(*iter)->setVelocity(0, 0);
								}

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

							calculateF(blackhole, *iter);
							(*iter)->update(deltaTime);
		} break;

		case CIRCLES: {
						  calculateF(blackhole, *iter);

						  //(*iter)->update(deltaTime);

		} break;

		case BLACKHOLE_: {
							 if (!playerIsDead)
							 {
								 (*iter)->setRadians(timeGetTime());
								 (*iter)->update(deltaTime);
							 }
							 else
							 {
								 (*iter)->setRadians(0);
							 }
		} break;
		}
	}
}

void Spacewar::DrawEntities() {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++){
		(*iter)->draw();
	}
}

int Spacewar::genScore(int combo) {
	return 2 + int(pow(combo, 1.5));
}

void Spacewar::collisions()
{
	VECTOR2 collisionVector;
	// if collision between ship and planet

	/*
	// if collision between ship and blackhole

	if (player->collidesWith(*blackhole, collisionVector))
	{
	if (!playerIsInvulnerable) {
	// bounce off blackhole
	player->bounce(collisionVector, *blackhole);
	player->damage(BLACKHOLE);

	if (player->getHealth() > 0)			// if player is alive
	{
	playerIsInvulnerable = true;
	playerInvulnerableTimer = 2.0f;
	}
	else if (player->getHealth() <= 0)		// if player is not alive
	{
	if (!playerIsDead)		// start death animation
	{
	playerIsDead = true;
	player->initialize(this, P_DEATH_WIDTH, P_DEATH_HEIGHT, P_DEATH_COLS, &p_deathTextures);
	player->setFrames(P_DEATH_START_FRAME, P_DEATH_END_FRAME);
	player->setCurrentFrame(P_DEATH_START_FRAME);
	player->setFrameDelay(P_DEATH_ANIMATION_DELAY);
	player->setLoop(false);
	//player->setRadians(prevAngle);
	player->setScale(0.5f);
	player->setRect();
	}
	}
	combo = 0;
	playerIsInvulnerable = true;
	playerInvulnerableTimer = 2.0f;
	}
	}
	*/

	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++)
	{
		Entity* entity = *iter;

		switch (entity->getObjectType())
		{
		case (BLACKHOLE_) :
		{
							  if (player->collidesWith(*entity, collisionVector))
							  {
								  player->bounce(collisionVector, *entity);
								  player->damage(BLACKHOLE);
								  playerIsInvulnerable = true;
								  playerInvulnerableTimer = 2.0f;
							  }
		}	break;

		case (CIRCLES) :
		{
						   if (player->collidesWith(*entity, collisionVector))
						   {
							   player->bounce(collisionVector, *entity);
							   player->damage(ENEMY);
							   playerIsInvulnerable = true;
							   playerInvulnerableTimer = 2.0f;
						   }
		}	break;

		case (TRIANGLES) :
		{
							 if (player->collidesWith(*entity, collisionVector))
							 {
								 player->bounce(collisionVector, *entity);
								 player->damage(ENEMY);
								 playerIsInvulnerable = true;
								 playerInvulnerableTimer = 2.0f;
							 }
		}



		}
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

double Spacewar::calculateF(Entity* e1, Entity* e2) {
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

	return force;
}
