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
	spriteData.scale = PLAYER_SCALING;
	spriteData.x = shipNS::X;
	spriteData.y = shipNS::Y;
	spriteData.rect.bottom = shipNS::HEIGHT;
	spriteData.rect.right = shipNS::WIDTH;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = PLAYER_START_FRAME;
	endFrame = PLAYER_END_FRAME;
	currentFrame = startFrame;
	radius = shipNS::WIDTH / 2.0;
	mass = shipNS::MASS;
	collisionType = entityNS::CIRCLE;
	health = PLAYER_HEALTH;

	// add the key and value pairs here
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_STUN, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_SLOW, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_INVINCIBLE, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_INVERTED, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_INVULNERABLE, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_FROZEN, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_ENLARGED, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_CANNOT_PICKUP, 0.0f));
	effectTimers.insert(std::pair<EffectType, float>(EFFECT_DEATH, 0.0f));
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

void Ship::triggerEffect(EffectType effect)
{
	switch (effect) {
	case EFFECT_ENLARGED:
	{
		if (this->hasEffect(EFFECT_ENLARGED)) {
			this->setScale(PLAYER_SCALING * 2);
		}
		else {
			this->setScale(PLAYER_SCALING);
		}
	}

	case EFFECT_STUN: {
		if (this->hasEffect(EFFECT_STUN))
			this->setVelocity(0, 0);
	} break;

	case EFFECT_INVINCIBLE: {

		if (this->hasEffect(EFFECT_INVINCIBLE))
		{
			// if the frames are not invincible, set the correct frames
			// else do nothing
			if (this->getCurrentFrame() < P_INVIN_START_FRAME || this->getCurrentFrame() > P_INVIN_END_FRAME)
			{
				this->setFrames(P_INVIN_START_FRAME, P_INVIN_END_FRAME);
				this->setCurrentFrame(P_INVIN_START_FRAME);
				this->setFrameDelay(P_INVIN_ANIMATION_DELAY);
				this->setLoop(P_INVIN_LOOP);
				this->setScale(P_INVIN_SCALE);
			}
		}
		else // no longer invincible
		{
			//	if the frames are not back to default, set the correct frames
			// else do nothing
			if (this->getCurrentFrame() > P_INVIN_START_FRAME || this->getCurrentFrame() < P_INVIN_END_FRAME)
			{
				this->setFrames(PLAYER_START_FRAME, PLAYER_END_FRAME);
				this->setCurrentFrame(PLAYER_START_FRAME);
				this->setScale(PLAYER_SCALING);
			}
		}

	} break;

	case EFFECT_SLOW: {
		if (this->hasEffect(EFFECT_SLOW)) {
			this->setVelocity(this->getVelocity().x / 1.05, this->getVelocity().y / 1.05);
		}
	} break;

	case EFFECT_INVULNERABLE: {

		// if player is alive
		if (this->getHealth() != 0)
		{
			if (this->hasEffect(EFFECT_INVULNERABLE))
			{
				// if the frames are not invulnerable, set the correct frames
				// else do nothing
				if (this->getCurrentFrame() < P_INVUL_START_FRAME || this->getCurrentFrame() > P_INVUL_END_FRAME)
				{
					this->setFrames(P_INVUL_START_FRAME, P_INVUL_END_FRAME);
					this->setCurrentFrame(P_INVUL_START_FRAME);
					this->setFrameDelay(P_INVUL_ANIMATION_DELAY);
					this->setLoop(P_INVUL_LOOP);
					this->setScale(P_INVUL_SCALE);
				}
			}
			else // no longer invulnerable
			{
				//	if the frames are not back to default, set the correct frames
				// else do nothing
				if (this->getCurrentFrame() > P_INVUL_START_FRAME || this->getCurrentFrame() < P_INVUL_END_FRAME)
				{
					this->setFrames(PLAYER_START_FRAME, PLAYER_END_FRAME);
					this->setCurrentFrame(PLAYER_START_FRAME);
					this->setScale(PLAYER_SCALING);
				}
			}
		}
	} break;

	case EFFECT_DEATH:
	{
		if (this->hasEffect(EFFECT_DEATH))
		{
			this->setVelocity(0, 0);
			this->setX(this->getX() + this->getWidth() / 2 * this->getScale() - (this->getWidth() / 2 * P_DEATH_SCALING));
			this->setY(this->getY() + this->getHeight() / 2 * this->getScale() - (this->getHeight() / 2 * P_DEATH_SCALING));

			// death animation
			if (this->currentFrame < P_DEATH_START_FRAME)
			{
				this->setFrames(P_DEATH_START_FRAME, P_DEATH_END_FRAME);
				this->setCurrentFrame(P_DEATH_START_FRAME);
				this->setFrameDelay(P_DEATH_ANIMATION_DELAY);
				this->setLoop(P_DEATH_LOOP);
				this->setScale(P_DEATH_SCALING);

				PlaySound(PLAYER_DEAD_SOUND, NULL, SND_ASYNC);
				printf("You DEAD son\n");
			}
		}
	}

	}
}