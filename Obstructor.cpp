#include "Obstructor.h"

Obstructor::Obstructor() : Entity(){
	spriteData.width = ObstructorNS::WIDTH;
	spriteData.height = ObstructorNS::HEIGHT;
	spriteData.x = ObstructorNS::X;
	spriteData.y = ObstructorNS::Y;
	spriteData.rect.bottom = ObstructorNS::HEIGHT;
	spriteData.rect.right = ObstructorNS::WIDTH;
	spriteData.scale = ObstructorNS::SCALING;
	velocity.x = 0;
	velocity.y = 0;
	startFrame = ObstructorNS::OBSTRUCTOR_START_FRAME;
	endFrame = ObstructorNS::OBSTRUCTOR_END_FRAME;
	currentFrame = startFrame;
	radius = ObstructorNS::WIDTH / 2.0f;
	mass = ObstructorNS::MASS;
	collisionType = entityNS::CIRCLE;
	o_type = SLOW_PLAYER;
}

bool Obstructor::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM){
	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

void Obstructor::draw(){
	Image::draw();
}

void Obstructor::update(float deltaTime){
	
}

void Obstructor::spawn(){
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

void Obstructor::damage(WEAPON weapon){
	switch (weapon)
	{
		case PLAYER:{
			this->setHealth(0);
		}	break;
	}
}

//returns the type of Obstructor [Blackhole, slow, stun, enlarge, invert]

/* ========================================	*/
/*			Private Methods					*/
/* ========================================	*/
//	Calcualte the type of Obstrutor 

void Obstructor::calculateObstructorType(){
	
}