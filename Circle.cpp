// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#include "Circle.h"

Circle::Circle() : Entity(){
	spriteData.width = CircleNS::WIDTH;
	spriteData.height = CircleNS::HEIGHT;
	spriteData.x = CircleNS::X;
	spriteData.y = CircleNS::Y;
	spriteData.rect.bottom = CircleNS::HEIGHT;
	spriteData.rect.right = CircleNS::WIDTH;
	spriteData.scale = CircleNS::SCALING;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = CircleNS::CIRCLE_START_FRAME;
	endFrame = CircleNS::CIRCLE_END_FRAME;
	currentFrame = startFrame;
	radius = CircleNS::WIDTH / 2.0f;
	mass = CircleNS::MASS;
	collisionType = entityNS::CIRCLE;
	this->setScale(CircleNS::SCALING);
}

bool Circle::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM){
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

void Circle::draw(){
	Image::draw();
}

void Circle::update(float deltaTime){
	Entity::update(deltaTime);
	spriteData.angle += deltaTime * CircleNS::ROTATION_RATE;  // rotate the ship
	spriteData.x += deltaTime * velocity.x;         // move ship along X 
	spriteData.y += deltaTime * velocity.y;         // move ship along Y

	// Bounce off walls
	if (spriteData.x > GAME_WIDTH - CircleNS::WIDTH * this->getScale())    // if hit right screen edge
	{
		spriteData.x = GAME_WIDTH - CircleNS::WIDTH * this->getScale();    // position at right screen edge
		velocity.x = -velocity.x;                   // reverse X direction
	}
	else if (spriteData.x < 0)                    // else if hit left screen edge
	{
		spriteData.x = 0;                           // position at left screen edge
		velocity.x = -velocity.x;                   // reverse X direction
	}
	if (spriteData.y > GAME_HEIGHT - CircleNS::HEIGHT * this->getScale())  // if hit bottom screen edge
	{
		spriteData.y = GAME_HEIGHT - CircleNS::HEIGHT * this->getScale();  // position at bottom screen edge
		velocity.y = -velocity.y;                   // reverse Y direction
	}
	else if (spriteData.y < 0)                    // else if hit top screen edge
	{
		spriteData.y = 0;                           // position at top screen edge
		velocity.y = -velocity.y;                   // reverse Y direction
	}
}
void Circle::spawn(){

	// Set CIRCLES settings
	setFrames(startFrame, endFrame);
	setCollisionRadius(getHeight() / 2);
	setVelocity(100, 200);
	setObjectType(OBJECT_TYPE_CIRCLE);
	setScale(0.2f);

	// random spawn on border
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
}

void Circle::damage(WEAPON weapon){
	switch (weapon) {
	case WEAPON_MISSILE: {
							 this->setHealth(0);
	} break;
	case WEAPON_EXPLOSION: {
							   this->setHealth(0);
	} break;
	case WEAPON_PLAYER: {
							this->setHealth(0);
	} break;
	}

	if (this->getHealth() < 0)
		this->setHealth(0);
}