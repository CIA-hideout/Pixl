#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#include "entity.h"
#include "constants.h"

namespace CircleNS {

	const float		MASS = 300.0f;
	const float		SCALING = 0.5f;
	const float		SPEED = 100;
	const float		ROTATION_RATE = (float)PI / 4;
	const int		WIDTH = 128;
	const int		HEIGHT = 128;
	const int		TEXTURE_COLS = 1;
	const int		CIRCLE_END_FRAME = 0;
	const int		CIRCLE_START_FRAME = 0;
	const int		X = GAME_WIDTH / 2 - WIDTH / 2;
	const int		Y = GAME_HEIGHT / 2 - HEIGHT / 2;
	const LPCWSTR	fileName = L"circle.png";

}

class Circle : public Entity {
public:

	Circle();

	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void spawn();
	void damage(WEAPON);

	ObjectType returnType();
};

#endif