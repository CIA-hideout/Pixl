// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#include "Missile.h"

/* Create a Missle Object,
* child of Entity Class
*/
Missile::Missile() : Entity() {
	spriteData.width = missileNS::WIDTH;
	spriteData.height = missileNS::HEIGHT;
	spriteData.scale = missileNS::SCALING;
	spriteData.rect.bottom = missileNS::HEIGHT;
	spriteData.rect.right = missileNS::WIDTH;
	velocity.x = 0;
	velocity.y = 0;
	currentFrame = startFrame;
	radius = missileNS::WIDTH / 2.0;
	mass = missileNS::MASS;
	collisionType = entityNS::BOX;
	objectType = OBJECT_TYPE_MISSILE;
	setScale(missileNS::SCALING);
}

/* 
 * Initialise Missle, by initalising an entity object
*/
bool Missile::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

/*
* Update the x and y coordinates,
* direction of missiles,
* velocity of missles
* by following the targeted object
*/
void Missile::update(float deltaTime) {

	Entity::update(deltaTime);
	float dx = target->getX() - this->getX();
	float dy = target->getY() - this->getY();

	if (dx > 0)
		this->setRadians(atan((dy / dx)));
	else if (dx < 0) {
		this->setRadians(PI + atan(dy / dx));
	}

	this->velocity.x = cos(this->getRadians()) * 250;
	this->velocity.y = sin(this->getRadians()) * 250;

	spriteData.x += deltaTime * velocity.x;
	spriteData.y += deltaTime * velocity.y;

}

/* 
 * Draw missile onto the screen
 */
void Missile::draw() {
	Image::draw();
}