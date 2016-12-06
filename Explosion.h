#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

#include "entity.h"

namespace explosionNS {
	const float	MASS = 0.0f;
	const float	SCALING = 4.0f;
	const float ANIMATION_DELAY = 0.025f;
	const int	WIDTH = 128;
	const int	HEIGHT = 128;
	const int	TEXTURE_COLS = 4;
	const int	EXPLOSION_END_FRAME = 15;
	const int	EXPLOSION_START_FRAME = 0;
}

class Explosion : public Entity {
public:
	Explosion();
	bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void draw();
private:
	float radius;
};

#endif
