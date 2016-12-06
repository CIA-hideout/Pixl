#ifndef _FREEZE_H_
#define _FREEZE_H_

#include "entity.h"

namespace freezeNS {
	const float	SCALING = 3.0f;
	const float ANIMATION_DELAY = 0.05f;
	const int	WIDTH = 128;
	const int	HEIGHT = 128;
	const int	TEXTURE_COLS = 4;
	const int	START_FRAME = 0;
	const int	END_FRAME = 12;
}

class Freeze : public Entity {
public:
	Freeze();
	bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void draw();
};

#endif
