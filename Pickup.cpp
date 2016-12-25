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
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

/*
* Draw the pickup object onto the screen.
*/
void Pickup::draw(){
	Image::draw();
}

void Pickup::update(float deltaTime){

}

/*
 * Spawn the pickup object randomly around the screen
 */
void Pickup::spawn(){

	if (this->getPickupType() != PICKUP_HEALTH)
		calculateObstructorDestructorType();

	// generate a random value for x and y
	int randX = (rand() % ((GAME_WIDTH - 2 * this->getWidth()) - this->getWidth() + 1) + this->getWidth());
	int randY = (rand() % ((GAME_HEIGHT - 2 * this->getHeight()) - this->getHeight() + 1) + this->getHeight());

	// spawn randomly in window
	this->setX(randX);
	this->setY(randY);
}

/*
 * Pickup takes damage, to be removed from screen
*/
void Pickup::damage(WEAPON weapon){
	switch (weapon)
	{
	case OBJECT_TYPE_PLAYER: {
								 // this->setHealth(0);
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
		PICKUP_DESTRUCTOR_MISSLES,

		PICKUP_DESTRUCTOR_FREEZE,
		PICKUP_DESTRUCTOR_FREEZE,

		PICKUP_DESTRUCTOR_INVINCIBILITY,
		PICKUP_DESTRUCTOR_INVINCIBILITY
	};

	//	Array containing types of Destructor

	if (isDestructor) {
		randNumber = rand() % 10; //Get a number from 0 - 9
		type = destructorArray[randNumber];
	}
	else // is an obstructor
	{
		randNumber = rand() % 9;	//Get a number from 0 - 8
		type = obstructorArray[randNumber];
	}

	if (isDestructor)
		this->setCurrentFrame(0);
	else
		this->setCurrentFrame(1);
}
