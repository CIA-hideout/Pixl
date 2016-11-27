#include "Blackhole.h"


Blackhole::Blackhole() : Entity() {

	spriteData.width = blackholeNS::WIDTH;
	spriteData.height = blackholeNS::HEIGHT;
	spriteData.scale = blackholeNS::SCALING;
	spriteData.x = blackholeNS::X;
	spriteData.y = blackholeNS::Y;
	spriteData.rect.bottom = blackholeNS::HEIGHT;
	spriteData.rect.right = blackholeNS::WIDTH;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = blackholeNS::BLACKHOLE_START_FRAME;
	endFrame = blackholeNS::BLACKHOLE_END_FRAME;
	currentFrame = startFrame;
	radius = blackholeNS::WIDTH / 2;
	mass = blackholeNS::MASS;
	collisionType = entityNS::CIRCLE;

}


bool Blackhole::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

void Blackhole::draw() {
	Image::draw();
}

void Blackhole::update(float deltaTime) {
	Entity::update(deltaTime);
	spriteData.angle += deltaTime * blackholeNS::ROTATION_RATE;
	spriteData.x += deltaTime * velocity.x;
	spriteData.y += deltaTime * velocity.y;
}

void Blackhole::damage(WEAPON weapon) {

}