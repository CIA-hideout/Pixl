#include "Pickup.h"

Pickup::Pickup() : Entity(){
	spriteData.width = PickupNS::WIDTH;
	spriteData.height = PickupNS::HEIGHT;
	spriteData.x = PickupNS::X;
	spriteData.y = PickupNS::Y;
	spriteData.rect.bottom = PickupNS::HEIGHT;
	spriteData.rect.right = PickupNS::WIDTH;
	spriteData.scale = PickupNS::SCALING;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = PickupNS::PICKUP_START_FRAME;
	endFrame = PickupNS::PICKUP_END_FRAME;
	currentFrame = startFrame;
	radius = PickupNS::WIDTH / 2.0f;
	mass = PickupNS::MASS;
	collisionType = entityNS::CIRCLE;
	PickupTypes type = OBSTRUCTOR_SLOW_PLAYER;
	boolean isDestructor = true;

	
}

bool Pickup::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM){
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

void Pickup::draw(){
	Image::draw();
}

void Pickup::update(float deltaTime){
	
}

void Pickup::spawn(){
	
	setFrames(startFrame, endFrame);
	setCollisionRadius(getHeight() / 2);
	setVelocity(0, 0);						// powerups don't move;
	setObjectType(CIRCLES);
	setScale(0.2f);

	//spawn randomly in window
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

void Pickup::damage(WEAPON weapon){
	switch (weapon)
	{
		case PLAYER:{
			this->setHealth(0);
		}	break;
	}
}

bool Pickup::isDestructor(){	//	Calculate whether is an Obstructor or Destructor
	srand(timeGetTime());
	int number = rand() % 100;

	if (number >= 80)			// 20% chance of a Obstructor
		destructor = false;	
	else						// 80% chance of a Destructor
		destructor = true;  

	calculatePickupType();

	return destructor;
}

/* ========================================	*/
/*			Private Methods					*/
/* ========================================	*/

//	Calculate the type of Obstrutor or Destructor the pickup is 
void Pickup::calculatePickupType(){
	
	srand(timeGetTime());
	int randNumber;

	//	Array containing types of Destructor
	PickupTypes destructorArray[] = {
		DESTRUCTOR_EXPLOSION,
		DESTRUCTOR_EXPLOSION,
		DESTRUCTOR_EXPLOSION,

		DESTRUCTOR_HOMING_MISSLES,
		DESTRUCTOR_HOMING_MISSLES,
		DESTRUCTOR_HOMING_MISSLES,

		DESTRUCTOR_FREEZE,
		DESTRUCTOR_FREEZE,

		DESTRUCTOR_INVULNERABILITY,
		DESTRUCTOR_INVULNERABILITY
	};

	// Array containing types of Obstructor
	PickupTypes obstructorArray[] = {
		OBSTRUCTOR_SLOW_PLAYER,
		OBSTRUCTOR_SLOW_PLAYER,

		OBSTRUCTOR_STUN_PLAYER,
		OBSTRUCTOR_STUN_PLAYER,

		OBSTRUCTOR_INVERT_CONTROLS,
		OBSTRUCTOR_INVERT_CONTROLS,

		OBSTRUCTOR_ENLARGE_PLAYER,
		OBSTRUCTOR_ENLARGE_PLAYER,

		OBSTRUCTOR_BLACKHOLE
	};

	if (this->getDestructor())
	{
		randNumber = rand() % 10;	//Get a number from 0 - 9
		type = destructorArray[randNumber];
	}
	else // is an obstructor
	{
		randNumber = rand() % 9;	//Get a number from 0 - 8
		type = obstructorArray[randNumber];
	}
}