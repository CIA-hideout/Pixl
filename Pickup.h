#ifndef _PICKUP_H_
#define _PICKUP_H_

#include "entity.h"
#include "constants.h"

namespace PickupNS {
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
}

enum	PickupType{
	//Declare Obstructors
	PICKUP_OBSTRUCTOR_INVERT_CONTROLS,
	PICKUP_OBSTRUCTOR_STUN_PLAYER,
	PICKUP_OBSTRUCTOR_SLOW_PLAYER,
	PICKUP_OBSTRUCTOR_ENLARGE_PLAYER,
	PICKUP_OBSTRUCTOR_BLACKHOLE,
	//Declare Destructors
	PICKUP_DESTRUCTOR_EXPLOSION,
	PICKUP_DESTRUCTOR_MISSLES,
	PICKUP_DESTRUCTOR_FREEZE,
	PICKUP_DESTRUCTOR_INVULNERABILITY,

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

	PickupType getPickupType() { return this->type; }
	bool getIsDestructor() { return this->isDestructor; }
	void calculateObstructorDestructorType();

private:

	PickupType type;
	boolean		isDestructor;
};

#endif