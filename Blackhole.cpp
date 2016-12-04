#include "Blackhole.h"

Blackhole::Blackhole() : Entity() {
	aliveTimer = 10.0f;
	collisionType = entityNS::CIRCLE;
	currentFrame = startFrame;
	endFrame = blackholeNS::BLACKHOLE_END_FRAME;
	mass = blackholeNS::MASS;
	objectType = OBJECT_TYPE_BLACKHOLE;
	radius = blackholeNS::WIDTH / 2;
	spriteData.height = blackholeNS::HEIGHT;
	spriteData.rect.bottom = blackholeNS::HEIGHT;
	spriteData.rect.right = blackholeNS::WIDTH;
	spriteData.scale = blackholeNS::SCALING;
	spriteData.width = blackholeNS::WIDTH;
	spriteData.x = blackholeNS::X;
	spriteData.y = blackholeNS::Y;
	startFrame = blackholeNS::BLACKHOLE_START_FRAME;
	setHealth(999);
	velocity.x = 0;
	velocity.y = 0;
}

bool Blackhole::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

void Blackhole::draw() {
	Image::draw();
}

void Blackhole::update(float deltaTime) {
	aliveTimer -= deltaTime;
	Entity::update(deltaTime);
	spriteData.angle += deltaTime * blackholeNS::ROTATION_RATE;
	setRadians(timeGetTime() / 600.0f);
	if (aliveTimer < 0.0f)
		setHealth(0);
}

void Blackhole::damage(WEAPON weapon) {
}