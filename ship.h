// Programming 2D Games Copyright (c) 2011 by: Charles Kelly Chapter 6 ship.h v1.0

#ifndef _SHIP_H                 // Prevent multiple definitions if this 
#define _SHIP_H                 // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "Pickup.h"
#include "constants.h"

namespace shipNS {

	const int WIDTH = 128;						// image width
	const int HEIGHT = 128;						// image height
	const int X = GAME_WIDTH / 2 - WIDTH / 2;   // location on screen
	const int Y = GAME_HEIGHT / 2 - HEIGHT / 2;
	const float ROTATION_RATE = 0;
	const float SPEED = 100;					// 100 pixels per second
	const float MASS = 300.0f;					// mass
	const int   TEXTURE_COLS = 1;
	const int   player_START_FRAME = 0;			// player starts at frame 0
	const int   player_END_FRAME = 0;			// player animation frames 0,1,2,3
	const float	SCALING = 0.5f;
}

// inherits from Entity class
class Ship : public Entity {
public:
	// constructor
	Ship();

	// inherited member functions
	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void damage(WEAPON);
	void effect(Pickup* pickUp);
};
#endif

