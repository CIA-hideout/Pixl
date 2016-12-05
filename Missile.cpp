#include "Missile.h"


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

bool Missile::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

void Missile::update(float deltaTime) {

	Entity::update(deltaTime);
	float dx = target->getX() - this->getX();
	float dy = target->getY() - this->getY();

	if (dx > 0)
		this->setRadians(atan((dy / dx)));
	else if (dx < 0) {
		this->setRadians(PI + atan(dy / dx));
	}

	this->velocity.x = cos(this->getRadians()) * 100;
	this->velocity.y = sin(this->getRadians()) * 100;

	spriteData.x += deltaTime * velocity.x;
	spriteData.y += deltaTime * velocity.y;

}

void Missile::draw() {
	Image::draw();
}