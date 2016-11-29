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
	
	enum			ObstructorType{
		INVERT_CONTROLS,
		STUN_PLAYER,
		SLOW_PLAYER,
		ENLARGE_PLAYER,
		BLACKHOLE
	};
	ObstructorType	type;
	//const LPCWSTR	fileName = L"obstructor.png";
}

class Obstructor : public Entity{
public:
	Obstructor();

	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void spawn();
	void damage(WEAPON);
	ObstructorNS::ObstructorType returnObstructorType();

private:
	void calculateObstructorType();
};

#endif