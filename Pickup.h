// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#ifndef _PICKUP_H_
#define _PICKUP_H_

#include "entity.h"
#include "constants.h"

namespace PickupNS {
	const float		MASS = 0.0f;
	const float		SCALING = 0.4f;
	const float		SPEED = 0;
	const float		ROTATION_RATE = (float)PI / 4;
	const int		WIDTH = 128;
	const int		HEIGHT = 128;
	const int		TEXTURE_COLS = 2;
	const int		PICKUP_END_FRAME = 0;
	const int		PICKUP_START_FRAME = 0;
	const int		X = GAME_WIDTH / 2 - WIDTH / 2;
	const int		Y = GAME_HEIGHT / 2 - HEIGHT / 2;
}

// declare what type of pickups are available in PickupType
enum PickupType{
	
	PICKUP_OBSTRUCTOR_INVERT_CONTROLS,
	PICKUP_OBSTRUCTOR_STUN_PLAYER,
	PICKUP_OBSTRUCTOR_SLOW_PLAYER,
	PICKUP_OBSTRUCTOR_ENLARGE_PLAYER,
	PICKUP_OBSTRUCTOR_BLACKHOLE,
	
	PICKUP_DESTRUCTOR_EXPLOSION,
	PICKUP_DESTRUCTOR_MISSLES,
	PICKUP_DESTRUCTOR_FREEZE,
	PICKUP_DESTRUCTOR_INVINCIBILITY,

	PICKUP_HEALTH
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
	void setPickUpType(PickupType pickupType);
	void setPickUpType();

	PickupType getPickupType() { return this->type; }			// returns the type of pickup (i.e. blackhole, health, stun, freeze)
	bool getIsDestructor() { return this->isDestructor; }		// returns true for destructor, false for obstructor
	void calculateObstructorDestructorType();					// calculate the type of pickup

private:

	PickupType type;
	boolean		isDestructor;
};

#endif