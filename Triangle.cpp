#include "Triangle.h"

Triangle::Triangle() : Entity() {

	spriteData.width = TriangleNS::WIDTH;
	spriteData.height = TriangleNS::HEIGHT;
	spriteData.x = TriangleNS::X;
	spriteData.y = TriangleNS::Y;
	spriteData.rect.bottom = TriangleNS::HEIGHT;
	spriteData.rect.right = TriangleNS::WIDTH;
	spriteData.scale = TriangleNS::SCALING;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = TriangleNS::TRIANGLE_START_FRAME;
	endFrame = TriangleNS::TRIANGLE_END_FRAME;
	currentFrame = startFrame;
	radius = TriangleNS::WIDTH / 2.0f;
	mass = TriangleNS::MASS;
	collisionType = entityNS::CIRCLE;

}

bool Triangle::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

void Triangle::draw() {
	Image::draw();
}

void Triangle::update(float deltaTime) {
	Entity::update(deltaTime);
	spriteData.angle += deltaTime * TriangleNS::ROTATION_RATE;  // rotate the ship
	spriteData.x += deltaTime * velocity.x;         // move ship along X 
	spriteData.y += deltaTime * velocity.y;         // move ship along Y

	// Bounce off walls
	if (spriteData.x > GAME_WIDTH - TriangleNS::WIDTH)    // if hit right screen edge
	{
		spriteData.x = GAME_WIDTH - TriangleNS::WIDTH;    // position at right screen edge
		velocity.x = -velocity.x;                   // reverse X direction
	}
	else if (spriteData.x < 0)                    // else if hit left screen edge
	{
		spriteData.x = 0;                           // position at left screen edge
		velocity.x = -velocity.x;                   // reverse X direction
	}
	if (spriteData.y > GAME_HEIGHT - TriangleNS::HEIGHT)  // if hit bottom screen edge
	{
		spriteData.y = GAME_HEIGHT - TriangleNS::HEIGHT;  // position at bottom screen edge
		velocity.y = -velocity.y;                   // reverse Y direction
	}
	else if (spriteData.y < 0)                    // else if hit top screen edge
	{
		spriteData.y = 0;                           // position at top screen edge
		velocity.y = -velocity.y;                   // reverse Y direction
	}
}


// To spawn a triangle outside the border
void Triangle::spawn()
{
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

	this->setScale(0.5);
}



void Triangle::damage(WEAPON weapon) {

}
