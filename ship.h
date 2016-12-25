// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

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
	const int   TEXTURE_COLS = 4;
}

// inherits from Entity class
class Ship : public Entity {
public:
	bool	playerDefaultTexture;				// true/false based on whether player has their starting texture or changed
	
	// constructor
	Ship();

	// inherited member functions
	virtual void draw();
	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);
	void update(float deltaTime);
	void damage(WEAPON);

	// GET functions
	bool getPlayerDefaultTexture(){ return playerDefaultTexture; };

	// SET functions
	void setPlayerDefaultTexture(bool PDT){ playerDefaultTexture = PDT; };
};
#endif

