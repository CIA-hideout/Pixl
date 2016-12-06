#ifndef _MISSILE_H_
#define _MISSILE_H_

#include "entity.h"

namespace missileNS {

	const int	WIDTH = 128;
	const int	HEIGHT = 32;
	const float ROTATION_RATE = PI / 4;
	const int	TEXTURE_COLS = 2;
	const int   MISSILE_START_FRAME = 0;
	const int   MISSILE_END_FRAME = 3;
	const float SPEED = 100;
	const float MASS = 100.0f;
	const float	SCALING = 0.25f;
	const float ANIMATION_DELAY = 0.1f;
	const bool LOOP = true;
	const ObjectType OBJECT_TYPE = OBJECT_TYPE_MISSILE;

}

class Missile : public Entity {
public:

	Missile();
	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	
	Entity* getTarget() { return this->target; }
	void setTarget(Entity* target) { this->target = target; }

private:

	Entity* target;

};

#endif