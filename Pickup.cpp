// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#include "Pickup.h"

/* Create a Pickup Object,
 * child of Entity Class
 */
Pickup::Pickup() : Entity(){
	spriteData.width = PickupNS::WIDTH;
	spriteData.height = PickupNS::HEIGHT;
	spriteData.rect.bottom = PickupNS::HEIGHT;
	spriteData.rect.right = PickupNS::WIDTH;
	spriteData.scale = PickupNS::SCALING;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = PickupNS::PICKUP_START_FRAME;
	endFrame = PickupNS::PICKUP_END_FRAME;
	currentFrame = 0;
	radius = PickupNS::WIDTH / 2.0f;
	mass = PickupNS::MASS;
	collisionType = entityNS::CIRCLE;
	objectType = OBJECT_TYPE_PICKUP;
}

/* 
 * Initialise Pickup, by initalising an entity object
 */

bool Pickup::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM){
	return (Entity::initialize(gamePtr, width, height, ncols, textureM));
}

/*
* Draw the pickup object onto the screen.
*/
void Pickup::draw(){
	Image::draw();
}

void Pickup::update(float deltaTime){
	Entity::update(deltaTime);
}

/*
 * Spawn the pickup object randomly around the screen
 */
void Pickup::spawn(){

	setFrames(startFrame, endFrame);
	setCollisionRadius(getHeight() / 2);
	setVelocity(0, 0);						// powerups don't move;
	setObjectType(OBJECT_TYPE_PICKUP);
	setScale(0.2f);

	//spawn randomly in window
	int side = rand() % 4;

	switch (side) {
		// left
	case 0: {
				this->setX(0 - rand() % GAME_WIDTH);
				this->setY(rand() % GAME_HEIGHT);
	} break;
		// top
	case 1: {
				this->setX(rand() % GAME_WIDTH);
				this->setY(-(rand() % GAME_HEIGHT));
	} break;
		// right
	case 2: {
				this->setX(GAME_WIDTH + GAME_WIDTH - rand() % GAME_WIDTH);
				this->setY(rand() % GAME_HEIGHT);
	} break;
		// bottom
	case 3: {
				this->setX(rand() % GAME_WIDTH);
				this->setY(GAME_HEIGHT + GAME_HEIGHT - rand() % GAME_HEIGHT);
	} break;
	}

	this->setX(rand() % GAME_WIDTH);
	this->setY(rand() % GAME_HEIGHT);

	this->setScale(0.5);
}

/*
 * Pickup takes damage, to be removed from screen
*/
void Pickup::damage(WEAPON weapon){
	switch (weapon)
	{
	case WEAPON_PLAYER: {
								 this->setHealth(0);
	}	break;
	}
}

/*
 * Set a Pickup Type
 * Decide if a Pickup is an obstructor (bad) or a destructor (good)
 */
void Pickup::setPickUpType(){
	int number = rand() % 100;
	if (number >= 80)
		isDestructor = false;
	else
		isDestructor = true;
}

/*
* Set a pickup type to a specific type of pickup instead of random
* Use for health pickup
*/
void Pickup::setPickUpType(PickupType pickupType) {
	this->type = pickupType;
	this->isDestructor = getPickupType() == PICKUP_DESTRUCTOR_EXPLOSION || getPickupType() == PICKUP_DESTRUCTOR_FREEZE || getPickupType() == PICKUP_DESTRUCTOR_INVINCIBILITY || getPickupType() == PICKUP_DESTRUCTOR_MISSLES;
}

/*
* Caluclate what kind of Obstrutor or Destructor the pickup is
* Is the destructor a bomb, or is it a missle
* Is the obstructor a black hole, or does it stun the player
*/
void Pickup::calculateObstructorDestructorType(){

	setPickUpType();
	int randNumber;

	// Array containing types of obstructor
	PickupType obstructorArray[] = {
		PICKUP_OBSTRUCTOR_SLOW_PLAYER,
		PICKUP_OBSTRUCTOR_SLOW_PLAYER,

		PICKUP_OBSTRUCTOR_STUN_PLAYER,
		PICKUP_OBSTRUCTOR_STUN_PLAYER,

		PICKUP_OBSTRUCTOR_INVERT_CONTROLS,
		PICKUP_OBSTRUCTOR_INVERT_CONTROLS,

		PICKUP_OBSTRUCTOR_ENLARGE_PLAYER,
		PICKUP_OBSTRUCTOR_ENLARGE_PLAYER,

		PICKUP_OBSTRUCTOR_BLACKHOLE
	};

	PickupType destructorArray[] = {
		PICKUP_DESTRUCTOR_EXPLOSION,
		PICKUP_DESTRUCTOR_EXPLOSION,
		PICKUP_DESTRUCTOR_EXPLOSION,

		PICKUP_DESTRUCTOR_MISSLES,
		PICKUP_DESTRUCTOR_MISSLES,

		PICKUP_DESTRUCTOR_FREEZE,
		PICKUP_DESTRUCTOR_FREEZE,

		PICKUP_DESTRUCTOR_INVINCIBILITY,
		PICKUP_DESTRUCTOR_INVINCIBILITY,

		PICKUP_HEALTH,
		PICKUP_HEALTH
	};

	//	Array containing types of Destructor

	if (isDestructor) {
		randNumber = rand() % 11; //Get a number from 0 - 11
		type = destructorArray[randNumber];
	}
	else // is an obstructor
	{
		randNumber = rand() % 9;	//Get a number from 0 - 9
		type = obstructorArray[randNumber];
	}

	if (isDestructor)
		this->setCurrentFrame(0);
	else
		this->setCurrentFrame(1);
}

void Pickup::setNewLocation()
{
	setX(minMaxRand_Pickup(getWidth(), GAME_WIDTH - 2 * getWidth()));
	setY(minMaxRand_Pickup(getHeight(), GAME_HEIGHT - 2 * getHeight()));
}

void Pickup::respawnPickup()
{
	calculateObstructorDestructorType();

	setNewLocation();
}

int Pickup::minMaxRand_Pickup(int min, int max) {
	return rand() % (max - min + 1) + min;
}
