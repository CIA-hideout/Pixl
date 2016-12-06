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
	spriteData.angle += deltaTime * TriangleNS::ROTATION_RATE;
	spriteData.x += deltaTime * velocity.x;
	spriteData.y += deltaTime * velocity.y;
}

// To spawn a triangle outside the border
void Triangle::spawn() {
	// Set Triangles Details
	setFrames(TriangleNS::TRIANGLE_START_FRAME, TriangleNS::TRIANGLE_END_FRAME);
	setCurrentFrame(TriangleNS::TRIANGLE_START_FRAME);
	setVelocity(10, 20);
	setObjectType(OBJECT_TYPE_TRIANGLE);

	// random spawn on border
	int side = rand() % 4;

	switch (side) {
		// left
	case 0: {
				this->setX(0 - rand() % 10);
				this->setY(rand() % GAME_HEIGHT);
	} break;
		// top
	case 1: {
				this->setX(rand() % GAME_WIDTH);
				this->setY(-rand() % 10);
	} break;
		// right
	case 2: {
				this->setX(GAME_WIDTH + rand() % 10);
				this->setY(rand() % GAME_HEIGHT);
	} break;
		// bottom
	case 3: {
				this->setX(rand() % GAME_WIDTH);
				this->setY(GAME_HEIGHT + rand() % 10);
	} break;
	}
}

void Triangle::damage(WEAPON weapon) {
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
