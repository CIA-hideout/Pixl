#ifndef _OBSTRUCTOR_H_
#define _OBSTRUCTOR_H_

#include "entity.h"
#include "constants.h"

namespace ObstructorNS{
	const float		MASS = 0.0f;
	const float		SCALING = 0.2f;
	const float		SPEED = 0;
	const float		ROTATION_RATE = (float)PI / 4;
	const int		WIDTH = 128;
	const int		HEIGHT = 128;
	const int		TEXTURE_COLS = 1;
	const int		OBSTRUCTOR_END_FRAME = 0;
	const int		OBSTRUCTOR_START_FRAME = 0;
	const int		X = GAME_WIDTH / 2 - WIDTH / 2;
	const int		Y = GAME_HEIGHT / 2 - HEIGHT / 2;
	
	//spawn rates for obstructors
	//const double	INVERT_SPAWNRATE = 3.0;
	//const double	STUN_SPAWNRATE = 3.0;
	//const double	SLOW_SPAWNRAETE = 3.0;
	//const double	ENLARGE_SPAWNRATE = 3.0;
	//const double	BLACKHOLE_SPAWNRATE = 0.5;

	//const LPCWSTR	fileName = L"obstructor.png";
}

enum			ObstructorType{
	INVERT_CONTROLS, 
	STUN_PLAYER,
	SLOW_PLAYER,
	ENLARGE_PLAYER,
	O_BLACKHOLE
};

class Obstructor : public Entity{
public:
	Obstructor();

	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void spawn();
	void damage(WEAPON);
	ObstructorType getObstructorType()
	{
		return o_type;
	}

private:
	ObstructorType o_type;
	void calculateObstructorType();
};

#endif