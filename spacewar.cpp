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

float	playerAccelerationRate;
float	playerDeccelerationRate;
float	playerTurnMultiplier;
float	playerInvulnerableTimer;
float	slowedTime, slowRadians;			// Used for slowing down blackhole rotation
float	deathAngle;							// the angle in radians at the point in time of the player's death
float	waveBufferTime;
float	pickupCoolDownTime;

int		playerMaxHealth, playerHealth;
int		playerScore, playerLevel;
int		combo;
int		waveTriangleCount, waveCircleCount, waveBossCount;
int		currentWave;

bool	controlsInverted, blackholeRunning, isStunned, isEnlarged;
bool	playerIsInvulnerable, playerIsDead, playerCanPickup;
bool	waveStarted;


DWORD	waveStartTime, nextWaveTime;
DWORD	baseTime, currTime;

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

	srand(timeGetTime());

	Game::initialize(hwnd);

	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	shipTextures.initialize(graphics, PLAYER_TEXTURE);
	p_deathTextures.initialize(graphics, PLAYER_DEATH_TEXTURE);
	triangleTextures.initialize(graphics, TRIANGLE_TEXTURE);
	circleTextures.initialize(graphics, CIRCLE_TEXTURE);
	blackHoleTexture.initialize(graphics, BLACKHOLE_TEXTURE);
	heartTexture.initialize(graphics, HEART_TEXTURE);
	fontTexture.initialize(graphics, FONT_TEXTURE);
	obstructorTexture.initialize(graphics, OBSTRUCTOR_TEXTURE);
	destructorTexture.initialize(graphics, DESTRUCTOR_TEXTURE);

	this->setGameState(GAME_STATE_MENU);

	// everything here should just be initialized onced

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

	menuFont = new Font();
	menuFont->initialize(this, 2048, 2048, 16, &fontTexture);
	menuFont->loadTextData(FONT_TEXTURE_INFO);
	menuFont->setHeight(128);
	menuFont->setWidth(128);

	int dx = GAME_WIDTH - GAME_WIDTH / 100;
	for (int i = 0; i < 10; i++) {
		Entity* heart = new Entity();
		heart->initialize(this, 128, 128, 2, &heartTexture);
		heart->setCurrentFrame(0);
		heart->setVelocity(0, 0);
		heart->setMass(0);
		heart->setRadians(0);
		heart->setScale(0.4);
		heart->setX(dx - heart->getWidth() * heart->getScale());
		heart->setY(GAME_HEIGHT - heart->getHeight() * heart->getScale() - 10);
		dx -= heart->getWidth() * heart->getScale();
		hearts.push_back(heart);
	}

	return;
}

void Spacewar::update() {

	switch (this->getGameState()) {

		case GAME_STATE_MENU: {
								  if (input->isKeyDown(0x20)) {

									  this->setGameState(GAME_STATE_GAME);
									  this->setIsRunning(true);

									  playerAccelerationRate = 3.5f;
									  playerDeccelerationRate = 0.005f;
									  playerTurnMultiplier = 3.5f;
									  playerInvulnerableTimer = 2.0f;

									  playerInvulnerableTimer = 2.0f;
									  playerIsInvulnerable = false;
									  playerIsDead = false;

									  playerHealth = 5;
									  playerMaxHealth = 10;

									  player = new Ship();
									  player->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
									  player->setCurrentFrame(shipNS::player_START_FRAME);
									  player->setScale(0.5f);
									  player->setVelocity(shipNS::SPEED, -shipNS::SPEED);
									  player->setObjectType(OBJECT_TYPE_PLAYER);
									  player->setRadians(0);
									  player->setVisible(true);
									  player->setVelocity(0, 0);
									  player->setX(GAME_WIDTH / 2 - player->getWidth() / 2 * player->getScale());
									  player->setY(GAME_HEIGHT / 2 - player->getHeight() / 2 * player->getScale());
									  player->setHealth(playerHealth);

									  this->addEntity(player);

									  healthPickup = new Pickup();
									  healthPickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &heartTexture);
									  healthPickup->setScale(0.5);
									  healthPickup->setVelocity(0, 0);
									  healthPickup->setX(100);
									  healthPickup->setY(100);
									  healthPickup->setObjectType(OBJECT_TYPE_PICKUP);
									  healthPickup->setPickUpType(PICKUP_HEALTH);
									  healthPickup->setCurrentFrame(1);

									  addEntity(healthPickup);

									  currentWave = 1;
									  waveBufferTime = 1.5f;
									  baseTime = timeGetTime();
								  }
		} break;
		case GAME_STATE_GAME: {

								  if (input->isKeyDown(0x1B)) {
									  this->setGameState(GAME_STATE_MENU);
									  this->setIsRunning(false);
									  entities.clear();
								  }

								  if (waveBufferTime > 0.0f) {
									  waveBufferTime -= deltaTime;
								  }

								  for (int i = 0; i < hearts.size(); i++) {
									  hearts[i]->setCurrentFrame(1);
									  hearts[i]->update(deltaTime);
								  }
								  for (int i = 0; i < hearts.size() - player->getHealth(); i++) {
									  hearts[i]->setCurrentFrame(0);
									  hearts[i]->update(deltaTime);
								  }

								  printf("player health: %.2f\n", player->getHealth());
		} break;
		case GAME_STATE_SETTING: {

		}

	}

}

void Spacewar::ai() {

	switch (this->getGameState()) {

		case GAME_STATE_MENU: {

		} break;
		case GAME_STATE_GAME: {

		} break;
		case GAME_STATE_SETTING: {

		} break;

	}
}

void Spacewar::render()
{
	graphics->spriteBegin();

	switch (this->getGameState()) {
		case GAME_STATE_MENU: {
								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth("P i x l .") / 2,
									  GAME_HEIGHT / 3,
									  "P i x l ."
									  );
								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth("Press space to begin") / 2,
									  GAME_HEIGHT / 2, "Press space to begin");
		} break;
		case GAME_STATE_GAME: {

								  if (waveBufferTime > 0.0f) {
									  menuFont->Print(
										  GAME_WIDTH / 2 - menuFont->getTotalWidth("wave" + std::to_string(currentWave)) / 2,
										  GAME_HEIGHT / 3,
										  "wave " + std::to_string(currentWave));
									  // buffer will not display anything for the first wave
									  if (currentWave != 1) {
										  DrawEntities();
										  ss.str("");
										  ss << std::fixed << std::setprecision(1) << (float)(timeGetTime() - baseTime) / 1000;
										  timeFont->Print(GAME_WIDTH / 2 - timeFont->getTotalWidth(ss.str()) / 2, 10, ss.str());
										  ss.str("x" + std::to_string(combo));
										  comboFont->Print(10, GAME_HEIGHT - comboFont->getHeight() * comboFont->getScale(), ss.str());
										  ss.str(std::to_string(playerScore));
										  scoreFont->Print(10, 10, ss.str());
										  for (std::vector<Entity*>::iterator iter = hearts.begin(); iter != hearts.end(); iter++) {
											  (*iter)->draw();
										  }
									  }
								  }
								  else {
									  DrawEntities();
									  ss.str("");
									  ss << std::fixed << std::setprecision(1) << (float)(timeGetTime() - baseTime) / 1000;
									  timeFont->Print(GAME_WIDTH / 2 - timeFont->getTotalWidth(ss.str()) / 2, 10, ss.str());
									  ss.str("x" + std::to_string(combo));
									  comboFont->Print(10, GAME_HEIGHT - comboFont->getHeight() * comboFont->getScale(), ss.str());
									  ss.str(std::to_string(playerScore));
									  scoreFont->Print(10, 10, ss.str());
									  for (std::vector<Entity*>::iterator iter = hearts.begin(); iter != hearts.end(); iter++) {
										  (*iter)->draw();
									  }
								  }

		} break;
		case GAME_STATE_SETTING: {

		} break;
	}

	graphics->spriteEnd();
}

void Spacewar::releaseAll() {
	shipTextures.onResetDevice();
	circleTextures.onResetDevice();
	p_deathTextures.onResetDevice();
	triangleTextures.onResetDevice();
	circleTextures.onResetDevice();
	blackHoleTexture.onResetDevice();
	fontTexture.onResetDevice();
	heartTexture.onResetDevice();
	Game::releaseAll();
	FreeConsole();
	return;
}

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

	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
		switch ((*iter)->getObjectType()) {
			case OBJECT_TYPE_PLAYER: {
										 if (input->isKeyDown(VK_UP)) {
											 (*iter)->setVelocity(
												 (cos((*iter)->getRadians() - PI / 2) * playerAccelerationRate + (*iter)->getVelocity().x),
												 (sin((*iter)->getRadians() - PI / 2) * playerAccelerationRate + (*iter)->getVelocity().y)
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

										 if (playerIsInvulnerable) {				// if player is not dead, set player sprite to no damage and blink animation.
											 playerInvulnerableTimer -= deltaTime;
											 if (playerInvulnerableTimer <= 0) {
												 playerIsInvulnerable = false;
											 }
										 }

										 if (!playerCanPickup) {
											 pickupCoolDownTime -= deltaTime;
											 if (pickupCoolDownTime <= 0) {
												 playerCanPickup = true;
											 }
										 }

										 if (blackholeRunning) {
											 calculateF(blackhole, player);
										 }

										 (*iter)->update(deltaTime);
			}
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

void Spacewar::collisions() {

	switch (this->getGameState())
	{
		case GAME_STATE_MENU: {

		} break;
		case GAME_STATE_GAME: {

								  VECTOR2 collisionVector;

								  for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++)
								  {
									  Entity* entity = *iter;
									  if (player->collidesWith(*entity, collisionVector)) {
										  switch (entity->getObjectType())
										  {

											  case OBJECT_TYPE_BLACKHOLE: {
																			  player->damage(WEAPON_BLACKHOLE);
																			  combo = 0;

											  }	break;

											  case OBJECT_TYPE_CIRCLES: {
																			if (!playerIsInvulnerable) {
																				player->bounce(collisionVector, *entity);
																				player->damage(WEAPON_CIRCLE);
																				playerIsInvulnerable = true;
																				playerInvulnerableTimer = 2.0f;
																				combo = 0;
																			}
											  }	break;

											  case OBJECT_TYPE_TRIANGLES: {
																			  if (!playerIsInvulnerable) {
																				  player->bounce(collisionVector, *entity);
																				  player->damage(WEAPON_TRIANGLE);
																				  playerIsInvulnerable = true;
																				  playerInvulnerableTimer = 2.0f;
																				  combo = 0;
																			  }
											  }	break;
											  case OBJECT_TYPE_PICKUP: {
																		   if (playerCanPickup) {
																			   Pickup* pickup_ = (Pickup*)entity;
																			   switch (pickup_->getPickupType()) {

																				   case PICKUP_DESTRUCTOR_EXPLOSION: {

																				   } break;
																				   case PICKUP_DESTRUCTOR_FREEZE: {

																				   } break;
																				   case PICKUP_DESTRUCTOR_INVULNERABILITY: {

																				   } break;
																				   case PICKUP_DESTRUCTOR_MISSLES: {

																				   } break;
																				   case PICKUP_HEALTH: {
																										   player->setHealth(player->getHealth() + 1);
																										   if (player->getHealth() > 10)
																											   player->setHealth(10);
																										   // no need to set pickup type for this pickup
																										   pickup_->setX(rand() % (int)(GAME_WIDTH + GAME_WIDTH * 0.5 / 2));
																										   pickup_->setX(rand() % (int)(GAME_HEIGHT + GAME_HEIGHT * 0.5 / 2));
																				   } break;
																				   case PICKUP_OBSTRUCTOR_BLACKHOLE: {
																														 // requires the blackhole to be spawned a set distance away
																														 float fx = pickup_->getX();
																														 float fy = pickup_->getY();
																				   } break;
																				   case PICKUP_OBSTRUCTOR_ENLARGE_PLAYER: {

																				   } break;
																				   case PICKUP_OBSTRUCTOR_INVERT_CONTROLS: {

																				   } break;
																				   case PICKUP_OBSTRUCTOR_SLOW_PLAYER: {

																				   } break;
																				   case PICKUP_OBSTRUCTOR_STUN_PLAYER: {

																				   } break;
																			   }
																			   playerCanPickup = false;
																			   pickupCoolDownTime = 1.0f;
																		   }

											  } break;
										  }
									  }

									  //deathX = player->getX();							// set value of x at point of death
									  //deathY = player->getY();							// set value of y at point of death
									  deathAngle = player->getRadians();		// set angle at point of death
									  // (test sprite requires additional 90 degrees)

									  //=============================================================================
									  // Check if player is dead.
									  // If player is dead, start death animation.
									  //=============================================================================
									  if (!isEntityAlive(player))		// if player is not alive
									  {
										  if (!playerIsDead)					// start death animation
										  {
											  playerIsDead = true;			// set to true for "player is already dead and animated, do not repeat."
											  player->initialize(this, P_DEATH_WIDTH, P_DEATH_HEIGHT, P_DEATH_COLS, &p_deathTextures);
											  player->setFrames(P_DEATH_START_FRAME, P_DEATH_END_FRAME);
											  player->setCurrentFrame(P_DEATH_START_FRAME);
											  player->setFrameDelay(P_DEATH_ANIMATION_DELAY);
											  player->setLoop(false);
											  //player->setX(deathX);
											  //player->setY(deathY);
											  player->setRadians(deathAngle);
											  player->setScale(0.5f);
											  player->setRect();
											  printf("%f\n", deathAngle);
										  }
									  }
								  }

		} break;
		case GAME_STATE_SETTING: {

		} break;
	}
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

bool Spacewar::isEntityAlive(Entity *e)
{
	if (e->getHealth() > 0)			// if player is alive
		return true;
	else
		return false;
}

