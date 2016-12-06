#include "Freeze.h"


Freeze::Freeze() : Entity() {

	spriteData.width = freezeNS::WIDTH;
	spriteData.height = freezeNS::HEIGHT;
	spriteData.rect.bottom = freezeNS::HEIGHT;
	spriteData.rect.right = freezeNS::WIDTH;
	spriteData.scale = freezeNS::SCALING;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = freezeNS::START_FRAME;
	endFrame = freezeNS::END_FRAME;
	currentFrame = startFrame;
	radius = freezeNS::WIDTH / 2.0f;
	collisionType = entityNS::CIRCLE;
	objectType = OBJECT_TYPE_FREEZE;
	frameDelay = 0.01f;
	health = 999;
}

bool Freeze::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

void Freeze::draw() {
	Image::draw();
}

void Freeze::update(float deltaTime) {
	Entity::update(deltaTime);
	if (currentFrame == endFrame) {
		setHealth(0);
	}
}