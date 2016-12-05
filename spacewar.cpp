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

#define SIGMOID(x, j) (1 / (1 + exp(-x / j)))

// just a few helper functions that does not deserve to be in a class
double calculateF(Entity* e1, Entity* e2);
void PrintEffect(Entity* entity, Font* effectFont);
bool isWaveOver(std::vector<Entity*> entities);
bool isTargeted(std::vector<Missile*> missiles, Entity* entity);

float	playerAccelerationRate;
float	playerDeccelerationRate;
float	playerTurnMultiplier;
float	slowedTime, slowRadians;			// Used for slowing down blackhole rotation
float	deathAngle;							// the angle in radians at the point in time of the player's death
float	waveBufferTime;
float	pickupCoolDownTime;
float	blackholeTimer;
float	frozenTime;							// Time for freezing enemy
float	velocityMultiplier;					// Used to reduce the velocity of the player

int		playerMaxHealth, playerHealth;
int		playerScore, playerLevel;
int		combo;
int		waveTriangleCount, waveCircleCount, waveBossCount;
int		currentWave;

bool	waveOver;
bool	playerIsDead, playerCanPickup;
bool	enemyIsFrozen;

DWORD	baseTime;

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
	missileTexture.initialize(graphics, MISSILE_TEXTURE);
	destructorObstructorTexture.initialize(graphics, DESTRUCTOR_OBSTRUCTOR_TEXTURE);
	explosionTexture.initialize(graphics, EXPLOSION_TEXTURE);

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

	effectFont = new Font();
	effectFont->initialize(this, 2048, 2048, 16, &fontTexture);
	effectFont->loadTextData(FONT_TEXTURE_INFO);
	effectFont->setHeight(128);
	effectFont->setWidth(128);
	effectFont->setScale(0.3);

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

								  playerIsDead = false;
								  enemyIsFrozen = false;

								  playerHealth = 5;
								  playerMaxHealth = 10;

								  velocityMultiplier = 1.05;
								  frozenTime = 5.0f;

								  player = new Ship();
								  player->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
								  player->setCurrentFrame(shipNS::player_START_FRAME);
								  player->setObjectType(OBJECT_TYPE_PLAYER);
								  player->setRadians(0);
								  player->setVisible(true);
								  player->setX(GAME_WIDTH / 2 - player->getWidth() / 2 * player->getScale());
								  player->setY(GAME_HEIGHT / 2 - player->getHeight() / 2 * player->getScale());
								  player->setHealth(playerHealth);

								  this->addEntity(player);

								  healthPickup = new Pickup();
								  healthPickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &heartTexture);
								  healthPickup->setPickUpType(PICKUP_HEALTH);
								  healthPickup->setCurrentFrame(0);
								  healthPickup->setX(rand() % (int)(healthPickup->getWidth() * healthPickup->getScale() + GAME_WIDTH));
								  healthPickup->setY(rand() % (int)(healthPickup->getHeight() * healthPickup->getScale() - GAME_HEIGHT));

								  addEntity(healthPickup);

								  for (int i = 0; i < 3; i++) {
									  Pickup* pickup = new Pickup();
									  pickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &destructorObstructorTexture);
									  pickup->calculatePickupType();
									  if (pickup->getIsDestructor())
										  pickup->setCurrentFrame(0);
									  else
										  pickup->setCurrentFrame(1);
									  pickup->setX(rand() % (int)(pickup->getWidth() * pickup->getScale() + GAME_WIDTH));
									  pickup->setY(rand() % (int)(pickup->getHeight() * pickup->getScale() - GAME_HEIGHT));

									  addEntity(pickup);
								  }

								  currentWave = 1;
								  // wave is over if and only if all enemies are not active
								  waveTriangleCount = (int)SIGMOID(currentWave, 5);

								  for (int i = 0; i <= 50; i++) {
									  Triangle* tri = new Triangle();
									  tri->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
									  tri->setObjectType(OBJECT_TYPE_TRIANGLE);
									  tri->setHealth(1);
									  tri->spawn();
									  addEntity(tri);
								  }

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

							  if (isWaveOver(entities)) {
								  currentWave++;
								  waveBufferTime = 1.5f;
								  waveTriangleCount = (int)SIGMOID(currentWave, 5);
								  for (int i = 0; i < waveTriangleCount; i++) {
									  Triangle* tri = new Triangle();
									  tri->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
									  tri->setObjectType(OBJECT_TYPE_TRIANGLE);
									  tri->setActive(true);
									  tri->setHealth(1);
									  tri->spawn();
									  addEntity(tri);
								  }
							  }

							  VECTOR2 collisionVector;
							  for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
								  (*iter)->update(deltaTime);
								  if ((*iter)->getTarget()->getActive()) {
									  if ((*iter)->getObjectType() == OBJECT_TYPE_MISSILE) {
										  if ((*iter)->collidesWith(*(*iter)->getTarget(), collisionVector)) {
											  if ((*iter)->getTarget()->getObjectType() == OBJECT_TYPE_TRIANGLE) {
												  Triangle* tri = (Triangle*)(*iter)->getTarget();
												  tri->damage(WEAPON_MISSILE);
												  (*iter)->setVisible(false);
												  (*iter)->setActive(false);
											  }
										  }
									  }
								  }
								  else {
									  // find a new target. if no new target exists, kill itself
									  for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
										  if (
											  ((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
											  (*iter_)->getActive() == true
											  ) {
											  (*iter)->setTarget(*iter_);
										  }
									  }
									  if (!(*iter)->getTarget()->getActive()) {
										  (*iter)->setActive(false);
									  }
								  }
							  }

							  // explosion collision detection
							  for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
								  if ((*iter)->getObjectType() == OBJECT_TYPE_EXPLOSION) {
									  for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
										  if (((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
											  (*iter_)->getActive() == true) {
											  if ((*iter_)->collidesWith(**iter, collisionVector)) {
												  if ((*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE) {
													  Triangle* triangle = (Triangle*)(*iter_);
													  triangle->damage(WEAPON_EXPLOSION);
												  }
											  }
										  }
									  }
								  }
							  }

							  KillEntities();
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

void Spacewar::render() {
	graphics->spriteBegin();

	switch (this->getGameState()) {
	case GAME_STATE_MENU: {
							  menuFont->Print(
								  GAME_WIDTH / 2 - menuFont->getTotalWidth("P i x l .") / 2,
								  GAME_HEIGHT / 3,
								  "P i x l ."
								  );
							  menuFont->Print(
								  GAME_WIDTH / 2 - menuFont->getTotalWidth("Press space to start") / 2,
								  GAME_HEIGHT / 2, "Press space to start");
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
									  PrintEffect(player, effectFont);
									  for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
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
								  float dy = 10;
								  PrintEffect(player, effectFont);
								  for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
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
											 (cos((*iter)->getRadians()) * playerAccelerationRate + (*iter)->getVelocity().x),
											 (sin((*iter)->getRadians()) * playerAccelerationRate + (*iter)->getVelocity().y)
											 );
									 }

									 if (input->isKeyDown(VK_DOWN)) {
										 (*iter)->setVelocity(
											 (cos((*iter)->getRadians()) * -playerAccelerationRate + (*iter)->getVelocity().x),
											 (sin((*iter)->getRadians()) * -playerAccelerationRate + (*iter)->getVelocity().y)
											 );
									 }

									 if (input->isKeyDown(VK_LEFT)) {
										 if (!(*iter)->hasEffect(EFFECT_INVERTED))
											 (*iter)->setRadians((*iter)->getRadians() - deltaTime * playerTurnMultiplier);
										 else
											 (*iter)->setRadians((*iter)->getRadians() + deltaTime * playerTurnMultiplier);
									 }

									 if (input->isKeyDown(VK_RIGHT)) {
										 if (!(*iter)->hasEffect(EFFECT_INVERTED))
											 (*iter)->setRadians((*iter)->getRadians() + deltaTime * playerTurnMultiplier);
										 else
											 (*iter)->setRadians((*iter)->getRadians() - deltaTime * playerTurnMultiplier);
									 }

									 if (input->isKeyDown(0x5A)) {
										 (*iter)->setVelocity(
											 (*iter)->getVelocity().x - (*iter)->getVelocity().x * playerDeccelerationRate * 3,
											 (*iter)->getVelocity().y - (*iter)->getVelocity().y * playerDeccelerationRate * 3);
									 }

									 (*iter)->setVelocity(
										 (*iter)->getVelocity().x - (*iter)->getVelocity().x * playerDeccelerationRate,
										 (*iter)->getVelocity().y - (*iter)->getVelocity().y * playerDeccelerationRate
										 );

									 if (!playerCanPickup) {
										 pickupCoolDownTime -= deltaTime;
										 if (pickupCoolDownTime <= 0) {
											 playerCanPickup = true;
										 }
									 }

									 // iterate through the various effect that the players have
									 // effects should be applied here
									 for (std::map<EffectType, float>::iterator iter_ = player->getEffectTimers()->begin(); iter_ != player->getEffectTimers()->end(); iter_++) {
										 if (iter_->second > 0.0f) {
											 iter_->second -= deltaTime;
											 switch (iter_->first) {
											 case EFFECT_STUN: {
																   if ((*iter)->hasEffect(EFFECT_STUN))
																	   (*iter)->setVelocity(0, 0);
											 } break;
											 case EFFECT_INVINCIBLE: {
											 } break;
											 case EFFECT_SLOW: {
																   if ((*iter)->hasEffect(EFFECT_SLOW)) {
																	   (*iter)->setVelocity((*iter)->getVelocity().x / velocityMultiplier, (*iter)->getVelocity().y / 1.05);
																   }
											 } break;
											 case EFFECT_INVULNERABLE: {
											 } break;
											 case EFFECT_FROZEN: {

											 } break;
											 }
										 }
									 }
									 if (player->hasEffect(EFFECT_STUN))
										 player->setVelocity(0, 0);
		} break;
		case OBJECT_TYPE_TRIANGLE: {
									   double dx, dy;

									   dx = player->getX() - (*iter)->getX();
									   dy = player->getY() - (*iter)->getY();

									   // 1, 4 quad
									   if (dx > 0)
										   (*iter)->setRadians(atan(dy / dx));
									   // 2, 3 quad
									   else if (dx < 0)
										   (*iter)->setRadians(PI + atan(dy / dx));

									   if (!playerIsDead) {
										   if (!enemyIsFrozen)	{
											   (*iter)->setVelocity(
												   cos((*iter)->getRadians()) * 50,
												   sin((*iter)->getRadians()) * 50
												   );
										   }
										   else {
											   (*iter)->setVelocity(0, 0);
										   }

									   }
									   else {
										   (*iter)->setVelocity(0, 0);
									   }

									   if (player->hasEffect(EFFECT_FROZEN)) {
										   (*iter)->setVelocity(0, 0);
									   }
		} break;
		case OBJECT_TYPE_MISSILE: {
		} break;
		case OBJECT_TYPE_BLACKHOLE: {
										calculateF(*iter, player);
		} break;
		}
		(*iter)->update(deltaTime);
	}
}

void Spacewar::KillEntities() {
	std::vector<Entity*>::iterator iter = entities.begin();
	while (iter != entities.end()) {
		if ((*iter)->getHealth() <= 0) {
			// whatever happends on death of the entity goes here
			switch ((*iter)->getObjectType()) {
			case OBJECT_TYPE_BLACKHOLE: {
											(*iter)->setActive(false);
											(*iter)->setVisible(false);
											iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_BOSS: {
									   (*iter)->setActive(false);
									   (*iter)->setVisible(false);
									   iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_CIRCLE: {
										 (*iter)->setActive(false);
										 (*iter)->setVisible(false);
										 iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_PICKUP: {
										 (*iter)->setActive(false);
										 (*iter)->setVisible(false);
										 iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_PLAYER: {
										 (*iter)->setActive(false);
										 (*iter)->setVisible(false);
										 iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_SQUARES: {
										  (*iter)->setActive(false);
										  (*iter)->setVisible(false);
										  iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_TRIANGLE: {
										   (*iter)->setActive(false);
										   (*iter)->setVisible(false);
										   iter = entities.erase(iter);
										   playerScore += genScore(++combo);
			} break;
			case OBJECT_TYPE_EXPLOSION: {
											(*iter)->setActive(false);
											(*iter)->setVisible(false);
											iter = entities.erase(iter);
			} break;
			}
		}
		else iter++;
	}

	std::vector<Missile*>::iterator iter_ = missiles.begin();
	while (iter_ != missiles.end()) {
		if (!(*iter_)->getActive()) {
			iter_ = missiles.erase(iter_);
		}
		else iter_++;
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
	std::vector<Entity*> tempVector;

	switch (this->getGameState())
	{
	case GAME_STATE_MENU: {
	} break;
	case GAME_STATE_GAME: {
							  VECTOR2 collisionVector;

							  for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
								  Entity* entity = *iter;
								  if (player->collidesWith(*entity, collisionVector)) {
									  switch (entity->getObjectType())
									  {
									  case OBJECT_TYPE_BLACKHOLE: {
																	  player->damage(WEAPON_BLACKHOLE);
																	  combo = 0;
									  }	break;

									  case OBJECT_TYPE_CIRCLE: {
																   if (!player->hasEffect(EFFECT_INVULNERABLE)) {
																	   player->damage(WEAPON_CIRCLE);
																	   player->getEffectTimers()->at(EFFECT_INVULNERABLE) = 2.0f;
																	   combo = 0;
																   }
									  }	break;

									  case OBJECT_TYPE_TRIANGLE: {
																	 if (!player->hasEffect(EFFECT_INVULNERABLE)) {
																		 player->damage(WEAPON_TRIANGLE);
																		 player->getEffectTimers()->at(EFFECT_INVULNERABLE) = 2.0f;
																		 combo = 0;
																	 }
									  }	break;
									  case OBJECT_TYPE_PICKUP: {
																   if (playerCanPickup) {
																	   Pickup* pickup_ = (Pickup*)entity;
																	   pickup_->getEffectDebug();
																	   switch (pickup_->getPickupType()) {
																	   case PICKUP_DESTRUCTOR_EXPLOSION: {
																											 Explosion* explosion = new Explosion();
																											 explosion->initialize(this, explosion->getWidth(), explosion->getHeight(), explosionNS::TEXTURE_COLS, &explosionTexture);
																											 explosion->setX(pickup_->getX() + pickup_->getWidth() * pickup_->getScale() / 2 - (explosion->getWidth() / 2 * explosion->getScale()));
																											 explosion->setY(pickup_->getY() + pickup_->getHeight() * pickup_->getScale() / 2 - (explosion->getHeight() / 2 * explosion->getScale()));
																											 explosion->setCollisionRadius(explosionNS::WIDTH / 2.0f);
																											 tempVector.push_back(explosion);

																											 pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																											 pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																											 pickup_->calculatePickupType();

																											 for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
																												 if (
																													 ((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
																													 (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
																													 (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
																													 (*iter_)->getActive() == true
																													 ) {
																												 }
																											 }
																	   } break;
																	   case PICKUP_DESTRUCTOR_FREEZE: {
																										  pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																										  pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																										  pickup_->calculatePickupType();
																										  player->getEffectTimers()->at(EFFECT_FROZEN) = 5.0f;
																	   } break;
																	   case PICKUP_DESTRUCTOR_INVULNERABILITY: {
																												   pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																												   pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																												   pickup_->calculatePickupType();
																	   } break;
																	   case PICKUP_DESTRUCTOR_MISSLES: {
																										   // get the enemies to target first
																										   std::vector<Entity*> tempVect;
																										   for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
																											   if (
																												   ((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
																												   (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
																												   (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
																												   (*iter_)->getActive() == true
																												   ) {
																												   tempVect.push_back(*iter_);
																											   }
																										   }

																										   // locates the target that the missile should go for
																										   // max 10
																										   for (int i = 0; i <= 10 && i < tempVect.size(); i++) {
																											   Missile* m = new Missile();
																											   m->initialize(this, 128, 32, 1, &missileTexture);
																											   m->setX(player->getX() + m->getWidth() / 2);
																											   m->setY(player->getY() + m->getHeight() / 2);
																											   m->setTarget(tempVect[i]);
																											   missiles.push_back(m);
																										   }
																										   pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																										   pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																										   pickup_->calculatePickupType();
																	   } break;
																	   case PICKUP_HEALTH: {
																							   player->setHealth(player->getHealth() + 1);
																							   if (player->getHealth() > 10)
																								   player->setHealth(10);
																							   playerScore += genScore(++combo);
																							   pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																							   pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																	   } break;
																	   case PICKUP_OBSTRUCTOR_BLACKHOLE: {
																											 pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																											 pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																											 pickup_->calculatePickupType();

																											 Blackhole* blackhole = new Blackhole();
																											 blackhole->initialize(this, blackholeNS::WIDTH, blackholeNS::HEIGHT, blackholeNS::TEXTURE_COLS, &blackHoleTexture);

																											 addEntity(blackhole);
																	   } break;
																	   case PICKUP_OBSTRUCTOR_ENLARGE_PLAYER: {
																												  pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																												  pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																												  pickup_->calculatePickupType();
																	   } break;
																	   case PICKUP_OBSTRUCTOR_INVERT_CONTROLS: {
																												   pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																												   pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																												   pickup_->calculatePickupType();
																												   player->getEffectTimers()->at(EFFECT_INVERTED) = 5.0f;
																	   } break;
																	   case PICKUP_OBSTRUCTOR_SLOW_PLAYER: {
																											   pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																											   pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																											   pickup_->calculatePickupType();
																											   player->getEffectTimers()->at(EFFECT_SLOW) = 5.0f;
																	   } break;
																	   case PICKUP_OBSTRUCTOR_STUN_PLAYER: {
																											   pickup_->setX(rand() % (int)(GAME_WIDTH - pickup_->getWidth() * pickup_->getScale()));
																											   pickup_->setY(rand() % (int)(GAME_HEIGHT - pickup_->getHeight() * pickup_->getScale()));
																											   pickup_->calculatePickupType();
																											   player->getEffectTimers()->at(EFFECT_STUN) = 5.0f;
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
										  player->setRadians(deathAngle);
										  player->setScale(0.5f);
										  player->setRect();
									  }
								  }
							  }

							  for (std::vector<Entity*>::iterator iter = tempVector.begin(); iter != tempVector.end(); iter++) {
								  addEntity(*iter);
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
	if (e->getHealth() > 0)
		return true;
	return false;
}

void PrintEffect(Entity* entity, Font* effectFont) {
	float dy = 10;
	for (std::map<EffectType, float>::iterator iter = entity->getEffectTimers()->begin(); iter != entity->getEffectTimers()->end(); iter++) {
		if (entity->hasEffect((*iter).first)) {
			ss.str("");
			switch ((*iter).first) {
			case EFFECT_INVERTED: {
									  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Inverted";
									  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
									  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_SLOW: {
								  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Slowed";
								  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
								  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_STUN: {
								  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Stunned";
								  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
								  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_INVULNERABLE: {
										  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Invulnerable";
										  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
										  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_FROZEN: {
									ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Frozen";
									effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
									dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			}
		}
	}
}

bool isWaveOver(std::vector<Entity*> entities) {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
		if ((
			(*iter)->getObjectType() == OBJECT_TYPE_BOSS ||
			(*iter)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
			(*iter)->getObjectType() == OBJECT_TYPE_CIRCLE ||
			(*iter)->getObjectType() == OBJECT_TYPE_BLACKHOLE) &&
			(*iter)->getActive() == true
			)
			return false;
	}
	return true;
}

bool isTargeted(std::vector<Missile*> missiles, Entity* entity) {
	for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
		if ((*iter)->getTarget() == entity)
			return true;
	}
	return false;
}