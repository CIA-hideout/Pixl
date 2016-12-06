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

enum PickupType{
	// Obstructors
	PICKUP_OBSTRUCTOR_INVERT_CONTROLS,
	PICKUP_OBSTRUCTOR_STUN_PLAYER,
	PICKUP_OBSTRUCTOR_SLOW_PLAYER,
	PICKUP_OBSTRUCTOR_ENLARGE_PLAYER,
	PICKUP_OBSTRUCTOR_BLACKHOLE,

	// Destructors
	PICKUP_DESTRUCTOR_EXPLOSION,
	PICKUP_DESTRUCTOR_MISSLES,
	PICKUP_DESTRUCTOR_FREEZE,
	PICKUP_DESTRUCTOR_INVULNERABILITY,

	// Others
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

	//return effects of pickups in a string format
	//enum is not supported in console
	//use for debug purposes
	void getEffectDebug();

	// SET Methods
	void setPickUpType();
	void setPickUpType(PickupType pickupType);

	//---------------
	// GET Methods

	// return `true` if pickup is a destructor
	// return `false` if otherwise
	bool getDestructor() { return this->destructor; }

	// return effect of pickups
	// Example: return `BLACKHOLE` if it is a blackhole
	PickupType getPickupType() { return this->type; }

	void calculatePickupType();			// set a random pickup effect


private:
	PickupType type;
	bool		destructor;

	//find if it is a pickup which inverts control, grow the player, stun the player,
	//spawns homing missles
	//void		calculatePickupType();
};

#endif
