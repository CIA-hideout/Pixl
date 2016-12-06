// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#include "spaceWar.h"
#include <stdlib.h>
#include <stdio.h>
#include <timeapi.h>
#include <sstream>
#include <iomanip>
#include <fstream>

// calculates the number of enemies
#define TRIANGLE_COUNT(x) (x * 0.15 * 16)
#define CIRCLE_COUNT(x) (x * 0.15 * 8)

// just a few helper functions that does not deserve to be in a class
void PrintEffect(Entity* entity, Font* effectFont);					// prints the effect that the player has
bool isWaveOver(std::vector<Entity*> entities);						// function to check if the wave is over based on 
bool isTargeted(std::vector<Missile*> missiles, Entity* entity);	// checks if an entitiy is targeted by a missile
int minMaxRand(int min, int max);									// generate a random integer based on minimum/maximum value

// values that will be used constantly so a might as well make them global

float	playerAccelerationRate;
float	playerDeccelerationRate;
float	playerTurnMultiplier;
float	slowedTime, slowRadians;									// Used for slowing down blackhole rotation
float	deathAngle;													// the angle in radians at the point in time of the player's death
float	waveBufferTime;												// a short peroid of time before the first wave and menu
float	blackholeTimer;												// time where the blackhole is left in the entities vector

int		playerMaxHealth, playerHealth;
int		playerScore, highscore;										// highscore will be written in a file when it is greater than the current highscore
int		combo, maxCombo;											// record combo
int		currentWave;

bool	waveOver;
bool	playerIsDead, playerCanPickup;
bool	playerDefaultTexture;
bool	beatenHighScore;

DWORD	baseTime;

std::stringstream ss;

Spacewar::Spacewar() {
}

Spacewar::~Spacewar() {
	releaseAll();           // call onLostDevice() for every graphics item
}

void Spacewar::initialize(HWND hwnd) {
	srand(timeGetTime());

	Game::initialize(hwnd);

	AllocConsole();

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	// Texture Initialization
	// Player
	shipTextures.initialize(graphics, PLAYER_TEXTURE);
	p_deathTextures.initialize(graphics, PLAYER_DEATH_TEXTURE);
	p_invulTextures.initialize(graphics, PLAYER_INVUL_TEXTURE);
	p_invinTextures.initialize(graphics, PLAYER_INVIN_TEXTURE);

	// Enemy
	triangleTextures.initialize(graphics, TRIANGLE_TEXTURE);
	circleTextures.initialize(graphics, CIRCLE_TEXTURE);
	blackHoleTexture.initialize(graphics, BLACKHOLE_TEXTURE);

	// Pickups
	missileTexture.initialize(graphics, MISSILE_TEXTURE);
	destructorObstructorTexture.initialize(graphics, DESTRUCTOR_OBSTRUCTOR_TEXTURE);
	explosionTexture.initialize(graphics, EXPLOSION_TEXTURE);

	// GUI
	heartTexture.initialize(graphics, HEART_TEXTURE);
	fontTexture.initialize(graphics, FONT_TEXTURE);

	this->setGameState(GAME_STATE_MENU);

	// Font initialization
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

	// positioning of hearts
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

	// read file to obtain highscore
	FILE* file;
	file = fopen(HIGHSCORE_FILE, "r");
	fscanf(file, "%d", &highscore);
	fclose(file);

	return;
}

void Spacewar::update() {
	switch (this->getGameState()) {
	case GAME_STATE_MENU: {
							  // start the game state if the spacebar is pressed
							  // variables that may have be changed in the previous state(s) will be set to 0/original values again
							  if (input->isKeyDown(SPACEBAR)) {
								  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);

								  beatenHighScore = false;
								  playerHealth = 3;
								  playerMaxHealth = 10;
								  playerIsDead = false;
								  playerDefaultTexture = true;
								  combo = maxCombo = playerScore = 0;

								  this->setGameState(GAME_STATE_GAME);

								  playerAccelerationRate = 3.5f;
								  playerDeccelerationRate = 0.005f;
								  playerTurnMultiplier = 3.5f;

								  // initialize stuff and add them into entites vector here
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
								  healthPickup->setX(minMaxRand(healthPickup->getWidth(), GAME_WIDTH - 2 * healthPickup->getWidth()));
								  healthPickup->setY(minMaxRand(healthPickup->getHeight(), GAME_HEIGHT - 2 * healthPickup->getHeight()));

								  addEntity(healthPickup);

								  // the pickups (obstructor/destructors)
								  for (int i = 0; i < 3; i++) {
									  Pickup* pickup = new Pickup();
									  pickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &destructorObstructorTexture);
									  pickup->calculateObstructorDestructorType();
									  if (pickup->getIsDestructor())
										  pickup->setCurrentFrame(0);
									  else
										  pickup->setCurrentFrame(1);
									  pickup->setX(minMaxRand(pickup->getWidth(), GAME_WIDTH - 2 * pickup->getWidth()));
									  pickup->setY(minMaxRand(pickup->getHeight(), GAME_HEIGHT - 2 * pickup->getHeight()));

									  addEntity(pickup);
								  }

								  currentWave = 1;

								  // the enemies
								  for (int i = 0; i < TRIANGLE_COUNT(currentWave); i++) {
									  Triangle* triangle = new Triangle();
									  triangle->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
									  triangle->setObjectType(OBJECT_TYPE_TRIANGLE);
									  triangle->setActive(true);
									  triangle->setHealth(1);
									  triangle->spawn();
									  addEntity(triangle);
								  }
								  for (int i = 0; i <= CIRCLE_COUNT(currentWave); i++) {
									  Circle* circle = new Circle();
									  circle->initialize(this, CircleNS::WIDTH, CircleNS::HEIGHT, CircleNS::TEXTURE_COLS, &circleTextures);
									  circle->setObjectType(OBJECT_TYPE_TRIANGLE);
									  circle->setHealth(1);
									  circle->spawn();
									  addEntity(circle);
								  }

								  waveBufferTime = 1.5f;
								  baseTime = timeGetTime();
							  }
	} break;
	case GAME_STATE_GAME: {
							  // return to menu
							  if (input->isKeyDown(ESC_KEY)) {
								  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);

								  this->setGameState(GAME_STATE_MENU);
								  entities.clear();
							  }

							  // buffer for the first wave
							  if (waveBufferTime > 0.0f) {
								  waveBufferTime -= deltaTime;
							  }

							  // update health bar increment/decrement
							  for (int i = 0; i < hearts.size(); i++) {
								  hearts[i]->setCurrentFrame(1);
								  hearts[i]->update(deltaTime);
							  }
							  for (int i = 0; i < hearts.size() - player->getHealth(); i++) {
								  hearts[i]->setCurrentFrame(0);
								  hearts[i]->update(deltaTime);
							  }

							  // checks if the wave is over and spawns new stuff
							  if (isWaveOver(entities)) {
								  currentWave++;
								  for (int i = 0; i < TRIANGLE_COUNT(currentWave); i++) {
									  Triangle* tri = new Triangle();
									  tri->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
									  tri->setObjectType(OBJECT_TYPE_TRIANGLE);
									  tri->setActive(true);
									  tri->setHealth(1);
									  tri->spawn();
									  addEntity(tri);
								  }
								  for (int i = 0; i <= CIRCLE_COUNT(currentWave); i++) {
									  Circle* circle = new Circle();
									  circle->initialize(this, CircleNS::WIDTH, CircleNS::HEIGHT, CircleNS::TEXTURE_COLS, &circleTextures);
									  circle->setObjectType(OBJECT_TYPE_TRIANGLE);
									  circle->setHealth(1);
									  circle->spawn();
									  addEntity(circle);
								  }
							  }

							  // checks the collision of missiles and entites here
							  VECTOR2 collisionVector;
							  for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
								  (*iter)->update(deltaTime);
								  // since target may die before the missile hits it, the misisle may have to find a new target
								  if ((*iter)->getTarget()->getActive()) {
									  if ((*iter)->getObjectType() == OBJECT_TYPE_MISSILE) {
										  if ((*iter)->collidesWith(*(*iter)->getTarget(), collisionVector)) {
											  switch ((*iter)->getTarget()->getObjectType()) {
											  case OBJECT_TYPE_TRIANGLE: {
																			 Triangle* triangle = (Triangle*)((*iter)->getTarget());
																			 triangle->damage(WEAPON_MISSILE);
																			 (*iter)->setVisible(false);
																			 (*iter)->setActive(false);
											  } break;
											  case OBJECT_TYPE_CIRCLE: {
																		   Circle* circle = (Circle*)((*iter)->getTarget());
																		   circle->damage(WEAPON_MISSILE);
																		   (*iter)->setVisible(false);
																		   (*iter)->setActive(false);
											  } break;
											  case OBJECT_TYPE_BOSS: {
											  } break;
											  }
										  }
									  }
								  }
								  // find new target. target should not be targeted by another missile
								  else {
									  for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
										  if (
											  ((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
											  (*iter_)->getActive() == true &&
											  !isTargeted(missiles, *iter_)
											  ) {
											  (*iter)->setTarget(*iter_);
										  }
									  }
									  // can't find any, kills itself
									  if (!(*iter)->getTarget()->getActive()) {
										  (*iter)->setActive(false);
									  }
								  }
							  }

							  // explosion detection
							  for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
								  if ((*iter)->getObjectType() == OBJECT_TYPE_EXPLOSION) {
									  for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
										  if (((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
											  (*iter_)->getActive() == true) {
											  if ((*iter_)->collidesWith(**iter, collisionVector)) {
												  switch ((*iter_)->getObjectType()) {
												  case OBJECT_TYPE_TRIANGLE: {
																				 Triangle* triangle = (Triangle*)(*iter_);
																				 triangle->damage(WEAPON_EXPLOSION);
												  } break;
												  case OBJECT_TYPE_CIRCLE: {
																			   Circle* circle = (Circle*)(*iter_);
																			   circle->damage(WEAPON_EXPLOSION);
												  } break;
												  case OBJECT_TYPE_BOSS: {
												  } break;
												  }
											  }
										  }
									  }
								  }
							  }

							  // update combo as the game progresses
							  if (combo > maxCombo)
								  maxCombo = combo;

							  // cleanup
							  KillEntities();
	} break;
	case GAME_STATE_SETTING: {
	} break;
	case GAME_STATE_GAMEOVER: {
								  if (input->isKeyDown(ESC_KEY)) {
									  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
									  printf("SELECT sound is played\n");

									  this->setGameState(GAME_STATE_MENU);

									  // empty entities
									  entities.clear();
								  }
	} break;
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
	case GAME_STATE_GAMEOVER: {
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
							  // mainly code on drawing entites and GUI
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
	case GAME_STATE_GAMEOVER: {
								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth("Game over") / 2,
									  GAME_HEIGHT / 3,
									  "Game over"
									  );

								  ss.str("");
								  ss << "score: " << playerScore << " Highest combo: " << maxCombo << " wave: " << currentWave;

								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth(ss.str()) / 2,
									  GAME_HEIGHT / 2.5,
									  ss.str()
									  );
								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth("Press esc to return to main menu") / 2,
									  GAME_HEIGHT / 2, "Press esc to return to main menu");

								  ss.str("");

								  // notifies the player if he has beaten the highscore
								  if (beatenHighScore) {
									  ss << "New highscore!";
									  menuFont->Print(
										  GAME_WIDTH / 2 - menuFont->getTotalWidth(ss.str()) / 2,
										  GAME_HEIGHT / 4,
										  ss.str()
										  );
								  }
								  // just show the highscore
								  else {
									  ss << "Highscore: " << highscore;
									  menuFont->Print(
										  GAME_WIDTH / 2 - menuFont->getTotalWidth(ss.str()) / 2,
										  GAME_HEIGHT - GAME_HEIGHT / 4,
										  ss.str()
										  );
								  }
	} break;
	}

	graphics->spriteEnd();
}

// release resources
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

// reset devices
void Spacewar::resetAll() {
	shipTextures.onResetDevice();
	triangleTextures.onResetDevice();
	shipTextures.onResetDevice();
	circleTextures.onResetDevice();
	p_deathTextures.onResetDevice();
	triangleTextures.onResetDevice();
	circleTextures.onResetDevice();
	blackHoleTexture.onResetDevice();
	fontTexture.onResetDevice();
	heartTexture.onResetDevice();
	Game::resetAll();
	FreeConsole();
	return;
}

// adds entity. static counter applies id
void Spacewar::addEntity(Entity* entity) {
	static int id_counter = 0;
	entity->setObjectId(id_counter);
	this->entities.push_back(entity);
	id_counter++;
}

void Spacewar::UpdateEntities() {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
		// update entites based on object types
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

									 // brakes
									 if (input->isKeyDown(0x5A)) {
										 (*iter)->setVelocity(
											 (*iter)->getVelocity().x - (*iter)->getVelocity().x * playerDeccelerationRate * 3,
											 (*iter)->getVelocity().y - (*iter)->getVelocity().y * playerDeccelerationRate * 3);
									 }

									 // natural decceleration
									 (*iter)->setVelocity(
										 (*iter)->getVelocity().x - (*iter)->getVelocity().x * playerDeccelerationRate,
										 (*iter)->getVelocity().y - (*iter)->getVelocity().y * playerDeccelerationRate
										 );

									 if (playerDefaultTexture)			// Prevents the default texture from animating
										 player->setCurrentFrame(player->getStartFrame());

									 // iterate through the various effect that the players have
									 // player effects should be applied here. enviroment effects can be recorded here though it is not recommended
									 for (std::map<EffectType, float>::iterator iter_ = player->getEffectTimers()->begin(); iter_ != player->getEffectTimers()->end(); iter_++) {
										 if (iter_->second > 0.0f) {
											 iter_->second -= deltaTime;
											 switch (iter_->first) {
											 case EFFECT_ENLARGED:
											 {
																	 if (player->hasEffect(EFFECT_ENLARGED)) {
																		 player->setScale(shipNS::SCALING * 2);
																	 }
																	 else {
																		 player->setScale(shipNS::SCALING);
																	 }
											 }
											 case EFFECT_STUN: {
																   if ((*iter)->hasEffect(EFFECT_STUN))
																	   (*iter)->setVelocity(0, 0);
											 } break;
											 case EFFECT_INVINCIBLE: {
																		 if ((*iter)->hasEffect(EFFECT_INVINCIBLE) && (*iter)->getCurrentFrame() == (*iter)->getStartFrame())
																		 {
																			 playerDefaultTexture = false;
																			 (*iter)->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, P_INVIN_COLS, &p_invinTextures);
																			 (*iter)->setFrames(P_INVIN_START_FRAME, P_INVIN_END_FRAME);
																			 (*iter)->setCurrentFrame(P_INVIN_START_FRAME);
																			 (*iter)->setFrameDelay(P_INVIN_ANIMATION_DELAY);
																			 (*iter)->setLoop(P_INVIN_LOOP);
																			 (*iter)->setScale(P_INVIN_SCALE);
																			 (*iter)->setRect();
																		 }
																		 else if (!(*iter)->hasEffect(EFFECT_INVINCIBLE) && playerDefaultTexture != true)
																		 {
																			 playerDefaultTexture = true;
																			 (*iter)->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
																			 (*iter)->setCurrentFrame(shipNS::player_START_FRAME);
																		 }
											 } break;
											 case EFFECT_SLOW: {
																   if ((*iter)->hasEffect(EFFECT_SLOW)) {
																	   (*iter)->setVelocity((*iter)->getVelocity().x / 1.05, (*iter)->getVelocity().y / 1.05);
																   }
											 } break;
											 case EFFECT_INVULNERABLE: {
																		   if ((*iter)->hasEffect(EFFECT_INVULNERABLE) && (*iter)->getCurrentFrame() == (*iter)->getStartFrame())
																		   {
																			   playerDefaultTexture = false;
																			   (*iter)->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, P_INVUL_COLS, &p_invulTextures);
																			   (*iter)->setFrames(P_INVUL_START_FRAME, P_INVUL_END_FRAME);
																			   (*iter)->setCurrentFrame(P_INVUL_START_FRAME);
																			   (*iter)->setFrameDelay(P_INVUL_ANIMATION_DELAY);
																			   (*iter)->setLoop(P_INVUL_LOOP);
																			   (*iter)->setScale(P_INVUL_SCALE);
																			   (*iter)->setRect();
																		   }
																		   else if (!(*iter)->hasEffect(EFFECT_INVULNERABLE) && playerDefaultTexture != true)
																		   {
																			   playerDefaultTexture = true;
																			   (*iter)->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
																			   (*iter)->setCurrentFrame(shipNS::player_START_FRAME);
																		   }
											 } break;
											 }
										 }
									 }
									 if (player->hasEffect(EFFECT_STUN))
										 player->setVelocity(0, 0);
		} break;
		case OBJECT_TYPE_TRIANGLE: {
									   // just tracks the player
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
										   cos((*iter)->getRadians()) * 50,
										   sin((*iter)->getRadians()) * 50
										   );
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
			// death only happens when health drops to 0
			// will be kept simple to prevent modifying the iterator while deleting an item
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
										 playerScore += genScore(++combo);
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

										 PlaySound(PLAYER_DEAD_SOUND, NULL, SND_FILENAME);
										 printf("You DEAD son\n");

										 FILE* file;

										 // if beaten highscore then write it to file
										 // expensive IO opration only happens once in the game loop
										 // there shouldn't be much impact
										 if (playerScore > highscore) {
											 beatenHighScore = true;
											 file = fopen(HIGHSCORE_FILE, "w");
											 highscore = playerScore;
											 fprintf(file, "%d", highscore);
											 fclose(file);
										 }

										 this->setGameState(GAME_STATE_GAMEOVER);
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

	// remove dead missiles
	std::vector<Missile*>::iterator iter_ = missiles.begin();
	while (iter_ != missiles.end()) {
		if (!(*iter_)->getActive()) {
			iter_ = missiles.erase(iter_);
		}
		else iter_++;
	}
}

// draw all entites
void Spacewar::DrawEntities() {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++){
		(*iter)->draw();
	}
}

// simple function to calculate what will be the score applied
int Spacewar::genScore(int combo) {
	return 2 + int(pow(combo, 1.5));
}

// a place where collision calculations takes place
void Spacewar::collisions() {

	// temp vector created to store entries that will be appended to entities after the iteration
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
									  // if player collides with whatever case:
									  switch (entity->getObjectType())
									  {
									  case OBJECT_TYPE_BLACKHOLE: {
																	  if (!player->hasEffect(EFFECT_INVULNERABLE) || !player->hasEffect((EFFECT_INVINCIBLE))){
																		  PlaySound(PLAYER_DAMAGE_SOUND, NULL, SND_ASYNC);
																		  printf("DAMAGE sound is played\n");
																	  }
																	  player->damage(WEAPON_BLACKHOLE);
																	  combo = 0;
									  }	break;

									  case OBJECT_TYPE_CIRCLE: {
																   Circle* circle = (Circle*)(*iter);

																   if (player->hasEffect(EFFECT_INVINCIBLE)) {
																	   circle->damage(WEAPON_PLAYER);
																   }
																   else if (!player->hasEffect(EFFECT_INVULNERABLE) && !player->hasEffect(EFFECT_INVINCIBLE)) {
																	   PlaySound(PLAYER_DAMAGE_SOUND, NULL, SND_ASYNC);
																	   printf("DAMAGE sound is played\n");

																	   player->damage(WEAPON_CIRCLE);
																	   player->getEffectTimers()->at(EFFECT_INVULNERABLE) = 2.4f;
																	   combo = 0;
																   }
									  }	break;

									  case OBJECT_TYPE_TRIANGLE: {
																	 Triangle* tri = (Triangle*)(*iter);

																	 if (player->hasEffect(EFFECT_INVINCIBLE)) {
																		 tri->damage(WEAPON_PLAYER);
																	 }
																	 else if (!player->hasEffect(EFFECT_INVULNERABLE) && !player->hasEffect(EFFECT_INVINCIBLE)) {
																		 PlaySound(PLAYER_DAMAGE_SOUND, NULL, SND_ASYNC);
																		 printf("DAMAGE sound is played\n");

																		 player->damage(WEAPON_TRIANGLE);
																		 player->getEffectTimers()->at(EFFECT_INVULNERABLE) = 2.4f;
																		 combo = 0;
																	 }
									  }	break;
									  case OBJECT_TYPE_PICKUP: {
																   // various pickup types
																   if (!player->hasEffect(EFFECT_CANNOT_PICKUP)) {
																	   Pickup* pickup_ = (Pickup*)entity;

																	   switch (pickup_->getPickupType()) {
																	   case PICKUP_DESTRUCTOR_EXPLOSION: {
																											 Explosion* explosion = new Explosion();
																											 explosion->initialize(this, explosion->getWidth(), explosion->getHeight(), explosionNS::TEXTURE_COLS, &explosionTexture);
																											 explosion->setX(pickup_->getX() + pickup_->getWidth() * pickup_->getScale() / 2 - (explosion->getWidth() / 2 * explosion->getScale()));
																											 explosion->setY(pickup_->getY() + pickup_->getHeight() * pickup_->getScale() / 2 - (explosion->getHeight() / 2 * explosion->getScale()));
																											 explosion->setCollisionRadius(explosionNS::WIDTH / 2.0f);
																											 tempVector.push_back(explosion);

																											 // play sound async to the game to avoid 'lag'
																											 PlaySound(PICKUP_EXPLODE_SOUND, NULL, SND_ASYNC);
																											 printf("Pickup EXPLODES!\n");

																											 pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																											 pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																											 pickup_->calculateObstructorDestructorType();
																	   } break;
																	   case PICKUP_DESTRUCTOR_FREEZE: {
																										  PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																										  pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																										  pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																										  pickup_->calculateObstructorDestructorType();
																										  player->getEffectTimers()->at(EFFECT_FROZEN) = 10.0f;
																	   } break;
																	   case PICKUP_DESTRUCTOR_INVINCIBILITY: {
																												 PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																												 pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																												 pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																												 pickup_->calculateObstructorDestructorType();
																												 player->getEffectTimers()->at(EFFECT_INVINCIBLE) = 10.0f;
																	   } break;
																	   case PICKUP_DESTRUCTOR_MISSLES: {
																										   // get the enemies to target first
																										   // discarded after this iteration
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

																										   // spawn 5 - 10 missiles
																										   for (int i = 0; i <= minMaxRand(5, 10) && i < tempVect.size(); i++) {
																											   Missile* m = new Missile();
																											   m->initialize(this, 128, 32, 1, &missileTexture);
																											   m->setX(player->getX() + m->getWidth() / 2);
																											   m->setY(player->getY() + m->getHeight() / 2);
																											   m->setTarget(tempVect[i]);
																											   missiles.push_back(m);
																										   }

																										   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																										   pickup_->calculateObstructorDestructorType();
																										   pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																										   pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																	   } break;
																	   case PICKUP_HEALTH: {
																							   // no need to reset heart type since there will always be one in a game
																							   PlaySound(PLAYER_PICKUP_HEART_SOUND, NULL, SND_ASYNC);
																							   printf("I play the HEART sound\n");

																							   player->setHealth(player->getHealth() + 1);
																							   if (player->getHealth() > 10)
																								   player->setHealth(10);
																							   playerScore += genScore(++combo);
																							   pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																							   pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																	   } break;
																	   case PICKUP_OBSTRUCTOR_BLACKHOLE: {
																											 // blackhole is a environmental effect.
																											 // blackhole that is stored in the entities vector will have effect on the gravity. will be killed when the timer reaches 0
																											 Blackhole* blackhole = new Blackhole();
																											 blackhole->initialize(this, blackholeNS::WIDTH, blackholeNS::HEIGHT, blackholeNS::TEXTURE_COLS, &blackHoleTexture);

																											 PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																											 pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));

																											 pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																											 pickup_->calculateObstructorDestructorType();

																											 addEntity(blackhole);
																	   } break;
																	   case PICKUP_OBSTRUCTOR_ENLARGE_PLAYER: {
																												  PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																												  pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																												  pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																												  pickup_->calculateObstructorDestructorType();
																												  player->getEffectTimers()->at(EFFECT_ENLARGED) = 5.0f;
																	   } break;
																	   case PICKUP_OBSTRUCTOR_INVERT_CONTROLS: {
																												   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																												   pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																												   pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																												   pickup_->calculateObstructorDestructorType();
																												   player->getEffectTimers()->at(EFFECT_INVERTED) = 5.0f;
																	   } break;
																	   case PICKUP_OBSTRUCTOR_SLOW_PLAYER: {
																											   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																											   pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																											   pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																											   pickup_->calculateObstructorDestructorType();
																											   player->getEffectTimers()->at(EFFECT_SLOW) = 5.0f;
																	   } break;
																	   case PICKUP_OBSTRUCTOR_STUN_PLAYER: {
																											   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																											   pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																											   pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));
																											   pickup_->calculateObstructorDestructorType();
																											   player->getEffectTimers()->at(EFFECT_STUN) = 5.0f;
																	   } break;
																	   }
																	   player->getEffectTimers()->at(EFFECT_CANNOT_PICKUP) = 0.5f;
																   }
									  } break;
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

// based on the formula GM1M2/R^2
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
			case EFFECT_ENLARGED: {
									  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Enlarged";
									  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
									  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_INVINCIBLE: {
										ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Invincible";
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

int minMaxRand(int min, int max) {
	return rand() % (max - min + 1) + min;
}
