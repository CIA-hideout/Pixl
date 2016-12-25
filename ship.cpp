// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 6 ship.cpp v1.0

#include "ship.h"

//=============================================================================
// Default Constructor
//=============================================================================
Ship::Ship() : Entity() {
	spriteData.width = shipNS::WIDTH;
	spriteData.height = shipNS::HEIGHT;
	spriteData.scale = player_SCALING;
	spriteData.x = shipNS::X;
	spriteData.y = shipNS::Y;
	spriteData.rect.bottom = shipNS::HEIGHT;
	spriteData.rect.right = shipNS::WIDTH;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = player_START_FRAME;
	endFrame = player_END_FRAME;
	currentFrame = startFrame;
	radius = shipNS::WIDTH / 2.0;
	mass = shipNS::MASS;
	collisionType = entityNS::CIRCLE;
	health = 3;

	// add the key and value pairs here
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_STUN, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_SLOW, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_INVINCIBLE, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_INVERTED, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_INVULNERABLE, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_FROZEN, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_ENLARGED, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_CANNOT_PICKUP, 0.0f));
}

bool Ship::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	bool error = Entity::initialize(gamePtr, width, height, ncols, textureM);
	this->setAnimationComplete(true);
	return error;
}

void Ship::draw() {
	Image::draw();              // draw ship
}

//=============================================================================
//									UPDATE
// typically called once per frame
// deltaTime is used to regulate the speed of movement and animation
//=============================================================================
void Ship::update(float deltaTime)
{
	// bouncing off the side of the screen
	Entity::update(deltaTime);
	spriteData.angle += deltaTime * shipNS::ROTATION_RATE;
	spriteData.x += deltaTime * velocity.x;
	spriteData.y += deltaTime * velocity.y;

	if (spriteData.x > GAME_WIDTH - shipNS::WIDTH * this->getScale()) {
		spriteData.x = GAME_WIDTH - shipNS::WIDTH * this->getScale();
		velocity.x = -velocity.x;
	}
	else if (spriteData.x < 0) {
		spriteData.x = 0;
		velocity.x = -velocity.x;
	}
	if (spriteData.y > GAME_HEIGHT - shipNS::HEIGHT  * this->getScale()) {
		spriteData.y = GAME_HEIGHT - shipNS::HEIGHT  * this->getScale();
		velocity.y = -velocity.y;
	}
	else if (spriteData.y < 0) {
		spriteData.y = 0;
		velocity.y = -velocity.y;
	}
}

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
							   if (this->health == 1)
								   this->setHealth(0);
							   else
								   this->setHealth(1);
	} break;
	}

	if (this->getHealth() < 0)
		this->setHealth(0);
}
