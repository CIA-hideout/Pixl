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

float			playerAccelerationRate;
float			playerDeccelerationRate;
float			playerTurnMultiplier;
float			playerInvulnerableTimer;			
float			slowedTime, slowRadians;			// Used for slowing down blackhole rotation
float			deathAngle;							// the angle in radians at the point in time of the player's death

//int				deathX, deathY;						// x and y value at the point in time of the player's death
int				playerMaxHealth, playerHealth;
int				playerScore, playerLevel;
int				combo;
int				waveTriangleCount, waveCircleCount, waveBossCount;
int				currentWave;

bool			controlsInverted, blackholeRunning, isStunned, isEnlarged;
bool			playerIsInvulnerable, playerIsDead;

DWORD			waveStartTime, nextWaveTime;

DWORD			baseTime, currTime;

// Pickups Variables
bool			isPlayerStun;						//	true if player gets STUN pickup
bool			isPlayerSlow;						//	true if player gets SLOW pickup
bool			isEnemyFrozen;						//	true if player gets FROZEN and let it go 

// In case different pickups have different duration set
float			obstructor_timer_stun;				// Duration of STUN pickup
float			obstructor_timer_slow;				// Duration of SLOW pickup
float			destructor_timer_frozen;			// Duration of LET IT GO


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

	AllocConsole();			// Brings up console

	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	int side;

	playerAccelerationRate = 3.5f;
	playerDeccelerationRate = 0.005f;
	playerTurnMultiplier = 3.5f;

	playerInvulnerableTimer = 2000.0f;

	playerIsInvulnerable = false;
	playerIsDead = false;

	playerHealth = 3;
	playerMaxHealth = 10;

	// Pickup Declarations
	isPlayerStun = false;						
	isPlayerSlow = false;						
	isEnemyFrozen = false;

	obstructor_timer_stun = 2000.0f;
	obstructor_timer_slow = 2000.0f;
	destructor_timer_frozen = 2000.0f;

	srand(timeGetTime());

	Game::initialize(hwnd); // throws GameError

	//=============================================================================
	// Main game textures
	//=============================================================================
	
	// Player
	shipTextures.initialize(graphics, PLAYER_TEXTURE);
	p_deathTextures.initialize(graphics, PLAYER_DEATH_TEXTURE);
	
	// Enemy
	triangleTextures.initialize(graphics, TRIANGLE_TEXTURE);
	circleTextures.initialize(graphics, CIRCLE_TEXTURE);

	// Pickups
	obstructorTexture.initialize(graphics, OBSTRUCTOR_TEXTURE);
	destructorTexture.initialize(graphics, DESTRUCTOR_TEXTURE);
	blackHoleTexture.initialize(graphics, BLACKHOLE_TEXTURE);
	
	// GUI/HUD
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
	player->setScale(0.5f);
	player->setVelocity(shipNS::SPEED, -shipNS::SPEED);
	player->setObjectType(PLAYER_SPRITE);
	player->setRadians(0);
	player->setHealth(3);

	addEntity(player);

	//blackhole = new Blackhole();
	//blackhole->initialize(this, blackholeNS::WIDTH, blackholeNS::HEIGHT, blackholeNS::TEXTURE_COLS, &blackHoleTexture);
	//blackhole->setFrames(blackholeNS::BLACKHOLE_START_FRAME, blackholeNS::BLACKHOLE_END_FRAME);
	//blackhole->setCurrentFrame(blackholeNS::BLACKHOLE_START_FRAME);
	//blackhole->setX(GAME_WIDTH / 2 - blackhole->getWidth() / 2 * blackhole->getScale());
	//blackhole->setY(GAME_HEIGHT / 2 - blackhole->getHeight() / 2 * blackhole->getScale());
	//blackhole->setVelocity(0, 0);
	//blackhole->setObjectType(BLACKHOLE_);
	//blackhole->setRadians(0);
	//blackhole->setScale(0.5f);

	//addEntity(blackhole);

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

	// Spawn Pickups
	for (int i = 0; i < 4; i++) {

		Pickup* pickup = new Pickup();
		pickup->setPickupType();		// randomly spawns an obstructor or detstructor

		if (pickup->isDestructor())
			pickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &destructorTexture);
		else
			pickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &obstructorTexture);
		
		pickup->spawn();		//	spawn pickup in its own method.
		addEntity(pickup);
	}

	// Spawn Triangles
	for (int i = 0; i < 0; i++) {

		Triangle* triangle = new Triangle();
		triangle->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
		triangle->setScale(0.5f);

		triangle->spawn();		//	spawn triangle in its own method.
		addEntity(triangle);
	}

	// Spawn Circles
	for (int i = 0; i < 0; i++){

		Circle* circle = new Circle();
		circle->initialize(this, CircleNS::WIDTH, CircleNS::HEIGHT, CircleNS::TEXTURE_COLS, &circleTextures);

		circle->spawn();
		addEntity(circle);
	}

	baseTime = timeGetTime();

	return;
}

void Spacewar::update() {

	//if (input->isKeyDown(0x42))
	//	blackhole->setMass(1);
	//else
	//	blackhole->setMass(9999999999999.0f);

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
									// if player is not stun, player move
									if (!isPlayerStun)
									{
										float speedMultipler = 1.0f;	//speed multipler to reduce/increase player speed
										
										if (isPlayerSlow)
											speedMultipler = 0.5f;		//reduce speed by 0.5

										

										if (input->isKeyDown(VK_UP)) {
											float x = (cos((*iter)->getRadians()) * playerAccelerationRate + (*iter)->getVelocity().x);
											float y = (sin((*iter)->getRadians()) * playerAccelerationRate + (*iter)->getVelocity().y);
											
											(*iter)->setVelocity(
												(cos((*iter)->getRadians()) * playerAccelerationRate + (*iter)->getVelocity().x) * speedMultipler,
												(sin((*iter)->getRadians()) * playerAccelerationRate + (*iter)->getVelocity().y) * speedMultipler

												);
											//printf("%.2f, %.2f\n", (*iter)->getVelocity().x, (*iter)->getVelocity().y);
										}

										if (input->isKeyDown(VK_LEFT)) {
											(*iter)->setRadians((*iter)->getRadians() - deltaTime * playerTurnMultiplier);
										}

										if (input->isKeyDown(VK_RIGHT)) {
											(*iter)->setRadians((*iter)->getRadians() + deltaTime * playerTurnMultiplier);
										}

										if (input->isKeyDown(Z_KEY)) {
											(*iter)->setVelocity(0, 0);
										}
										(*iter)->setVelocity(
											((*iter)->getVelocity().x - (*iter)->getVelocity().x * playerDeccelerationRate),
											((*iter)->getVelocity().y - (*iter)->getVelocity().y * playerDeccelerationRate)
											);
									}
									
									
									if (playerIsInvulnerable) {				// if player is not dead, set player sprite to no damage and blink animation.
										playerInvulnerableTimer -= deltaTime;
										if (playerInvulnerableTimer < 0) {
											playerIsInvulnerable = false;
										}
									}

									if (isPlayerStun) {

										obstructor_timer_stun -= deltaTime;
										if (obstructor_timer_stun < 0){
											isPlayerStun = false;
											
										}
									}

									if (isPlayerSlow){
										obstructor_timer_slow -= deltaTime;
										if (obstructor_timer_slow < 0)
										{
											isPlayerSlow = false;
										}
									}
								}

								else
								{
									(*iter)->setVelocity(0, 0);
								}
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

							if (!playerIsDead)
							{
								(*iter)->setVelocity(
									cos((*iter)->getRadians()) * 50,
									sin((*iter)->getRadians()) * 50
									);

								// Freeze enemy triangles
								if (isEnemyFrozen)
								{
									(*iter)->setVelocity(0, 0);
									destructor_timer_frozen -= deltaTime;
									
									if (destructor_timer_frozen < 0)
									{
										isEnemyFrozen = false;
									}
								}
							}
							else
							{
								(*iter)->setVelocity(0, 0);
							}

		} break;

		case CIRCLES: {
							// Freeze enemy circles
							if (isEnemyFrozen)
							{
								(*iter)->setVelocity(0, 0);
								destructor_timer_frozen -= deltaTime;

								if (destructor_timer_frozen < 0)
								{
									isEnemyFrozen = false;
								}
							}

							if (playerIsDead)
							  {
								  (*iter)->setVelocity(0, 0);
							  }
							
		} break;

		case BLACKHOLE_: {
							 int comparedValue = 0;

							 if (!playerIsDead)
							 {
								 (*iter)->setRadians(timeGetTime());
								 (*iter)->update(deltaTime);
								 slowedTime = timeGetTime() - 50;			// it is 50 "frames" slower
								 slowRadians = timeGetTime();				// set to start where player dies
							 }
							 else
							 {
								 comparedValue = timeGetTime() - slowedTime;
								 
								 if (comparedValue >= 50)					// every 50 radians,
								 {
									 slowRadians += 10.0f;					// increase radians by 10 (5x slower)
									 slowedTime = timeGetTime();			// reset slowed time so it is caught up with the current time
									 (*iter)->setRadians(slowRadians);		// rotate circle based on the slowed version of the radians
								 }
							 }
		} break;

		case PICKUPS:
		{
			// I DON'T KNOW WHAT TO DO WITH THIS
		}	break;
		}

		//if (((*iter)->getObjectType()) != BLACKHOLE_)
		//	calculateF(blackhole, (*iter));						// calculate black hole's attraction here

		(*iter)->update(deltaTime);		
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

//=============================================================================
// Check for entity collisions with player.
// Damage player accordingly.
//=============================================================================
void Spacewar::collisions()
{
	VECTOR2 collisionVector;

	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++)
	{
		Entity* entity = *iter;

		if (!playerIsInvulnerable)
		{
			switch (entity->getObjectType())
			{
				case (BLACKHOLE_) :
				{
										if (player->collidesWith(*entity, collisionVector))
										{
											player->bounce(collisionVector, *entity);
											player->damage(BLACKHOLE);
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
				}	break;

				case (PICKUPS):
				{
										if (player->collidesWith(*entity, collisionVector)){
											Pickup* pickup = (Pickup*)(*iter);
											pickup->getEffectDebug();
											
											// Different pickups does different stuff
											switch (pickup->getEffect())
											{
												// All the Obstructors

											case OBSTRUCTOR_INVERT_CONTROLS:{

											} break;

												//STUN : Player cannot move
											case OBSTRUCTOR_STUN_PLAYER:{
												isPlayerStun = true;
												obstructor_timer_stun = 5.0f;
											} break;

											case OBSTRUCTOR_SLOW_PLAYER:{
												isPlayerSlow = true;
												obstructor_timer_slow = 5.0f;
											}break;

											case OBSTRUCTOR_ENLARGE_PLAYER:{

											}break;

											case OBSTRUCTOR_BLACKHOLE:{

											}break;


												// All the Desstructors

											case DESTRUCTOR_EXPLOSION:{

											}break;

											case DESTRUCTOR_HOMING_MISSLES:{

											}break;

											case DESTRUCTOR_FREEZE:{
												isEnemyFrozen = true;
												destructor_timer_frozen = 5.0;
											}break;

											case DESTRUCTOR_INVULNERABILITY:{

											}break;
											}
										}
										
				}break;
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
				player->setScale(P_DEATH_SCALE);
				//player->setX(deathX);
				//player->setY(deathY);
				player->setRadians(deathAngle);
				player->setRect();
				printf("%f\n", deathAngle);
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

bool Spacewar::isEntityAlive(Entity *e)
{
	if (e->getHealth() > 0)			// if player is alive
		return true;
	else
		return false;
}

