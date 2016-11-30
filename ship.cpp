// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 6 ship.cpp v1.0

#include "ship.h"

//=============================================================================
// default constructor
//=============================================================================
Ship::Ship() : Entity()
{
	spriteData.width = shipNS::WIDTH;           // size of player
	spriteData.height = shipNS::HEIGHT;
	spriteData.scale = shipNS::SCALING;
	spriteData.x = shipNS::X;                   // location on screen
	spriteData.y = shipNS::Y;
	spriteData.rect.bottom = shipNS::HEIGHT;    // rectangle to select parts of an image
	spriteData.rect.right = shipNS::WIDTH;
	velocity.x = 0;                             // velocity X
	velocity.y = 0;                             // velocity Y
	startFrame = shipNS::player_START_FRAME;     // first frame of ship animation
	endFrame = shipNS::player_END_FRAME;     // last frame of ship animation
	currentFrame = startFrame;
	radius = shipNS::WIDTH / 2.0;
	mass = shipNS::MASS;
	collisionType = entityNS::CIRCLE;
	health = 5;
}

//=============================================================================
// Initialize the Ship.
// Post: returns true if successful, false if failed
//=============================================================================
bool Ship::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the ship
//=============================================================================
void Ship::draw() {
	Image::draw();              // draw ship
}

//=============================================================================
// update
// typically called once per frame
// deltaTime is used to regulate the speed of movement and animation
//=============================================================================
void Ship::update(float deltaTime)
{
	Entity::update(deltaTime);
	spriteData.angle += deltaTime * shipNS::ROTATION_RATE;
	spriteData.x += deltaTime * velocity.x;
	spriteData.y += deltaTime * velocity.y;

	if (spriteData.x > GAME_WIDTH - shipNS::WIDTH * this->getScale()) {
		spriteData.x = GAME_WIDTH - shipNS::WIDTH * this->getScale();
		velocity.x = -velocity.x;
	}
	else if (spriteData.x < 0)
	{
		spriteData.x = 0;
		velocity.x = -velocity.x;
	}
	if (spriteData.y > GAME_HEIGHT - shipNS::HEIGHT  * this->getScale())
	{
		spriteData.y = GAME_HEIGHT - shipNS::HEIGHT  * this->getScale();
		velocity.y = -velocity.y;
	}
	else if (spriteData.y < 0)
	{
		spriteData.y = 0;
		velocity.y = -velocity.y;
	}
}

//=============================================================================
// damage
//=============================================================================
void Ship::damage(WEAPON weapon) {

	switch (weapon)
	{
		case WEAPON_CIRCLE: {
								this->setHealth(this->getHealth() - 1);
		} break;
		case WEAPON_TRIANGLE: {
								  this->setHealth(this->getHealth() - 1);
		} break;
		case WEAPON_BOSS: {
							  this->setHealth(this->getHealth() - 2);
		} break;
		case WEAPON_BLACKHOLE: {
								   this->setHealth(0);
		} break;
	}

	if (this->getHealth() < 0)
		this->setHealth(0);
}
