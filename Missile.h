// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#ifndef _MISSILE_H_
#define _MISSILE_H_

#include "entity.h"

namespace missileNS {

	const int	WIDTH = 128;
	const int	HEIGHT = 32;
	const float ROTATION_RATE = PI / 4;
	const int	TEXTURE_COLS = 1;
	const int   MISSILE_START_FRAME = 0;
	const int   MISSILE_END_FRAME = 0;
	const float SPEED = 100;
	const float MASS = 100.0f;
	const float	SCALING = 0.25f;
	const ObjectType OBJECT_TYPE = OBJECT_TYPE_MISSILE;

}

class Missile : public Entity {
public:

	Missile();
	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	
	Entity* getTarget() { return this->target; }					// return the targeted object
	void setTarget(Entity* target) { this->target = target; }		// set the target object to be followed by missile

private:

	Entity* target;

};

#endif