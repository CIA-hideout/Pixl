#ifndef _BLACK_HOLE_
#define _BLACK_HOLE_

#include "entity.h"
#include "Blackhole.h"

namespace blackholeNS {

	const float			MASS = 9999999999999.0f;
	const float			SCALING = 0.5f;
	const float			SPEED = 0;
	const float			ROTATION_RATE = (float)PI / 4;
	const int			WIDTH = 128;
	const int			HEIGHT = 128;
	const int			TEXTURE_COLS = 1;
	const int			BLACKHOLE_END_FRAME = 0;
	const int			BLACKHOLE_START_FRAME = 0;
	const int			X = GAME_WIDTH / 2 - WIDTH / 2;
	const int			Y = GAME_HEIGHT / 2 - HEIGHT / 2;
	

}

class Blackhole : public Entity {
public:

	Blackhole();

	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void damage(WEAPON);

	ObjectType returnType();

private: 

	float aliveTimer;
};

#endif