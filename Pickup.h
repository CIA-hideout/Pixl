#ifndef _PICKUP_H_
#define _PICKUP_H_

#include "entity.h"
#include "constants.h"

namespace PickupNS{
	const float		MASS = 0.0f;
	const float		SCALING = 0.2f;
	const float		SPEED = 0;
	const float		ROTATION_RATE = (float)PI / 4;
	const int		WIDTH = 128;
	const int		HEIGHT = 128;
	const int		TEXTURE_COLS = 1;
	const int		PICKUP_END_FRAME = 0;
	const int		PICKUP_START_FRAME = 0;
	const int		X = GAME_WIDTH / 2 - WIDTH / 2;
	const int		Y = GAME_HEIGHT / 2 - HEIGHT / 2;
	//const LPCWSTR	fileName = L"obstructor.png";
}

enum	PickupTypes{
	//Declare Obstructors
	OBSTRUCTOR_INVERT_CONTROLS, 
	OBSTRUCTOR_STUN_PLAYER,
	OBSTRUCTOR_SLOW_PLAYER,
	OBSTRUCTOR_ENLARGE_PLAYER,
	OBSTRUCTOR_BLACKHOLE,

	//Declare Destructors
	DESTRUCTOR_EXPLOSION,
	DESTRUCTOR_HOMING_MISSLES,
	DESTRUCTOR_FREEZE,
	DESTRUCTOR_INVULNERABILITY
};

class Pickup : public Entity{
public:
	Pickup();

	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void spawn();
	void damage(WEAPON);


	// GET and SET Methods
	void setPickUpType();

	boolean getPickupType(){
		return isDestructor;
	}


	// return effect of pickups
	// Example: return `blackhole`
	PickupTypes getEffect(){
		return type;
	}



private:
	PickupTypes type;
	boolean		isDestructor;

	//find if it is a pickup which inverts control, grow the player, stun the player,
	//spawns homing missles
	void calculateObstructorDestructorType();
};

#endif