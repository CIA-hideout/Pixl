// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 6 ship.cpp v1.0

#include "ship.h"

//=============================================================================
// default constructor
//=============================================================================
Ship::Ship() : Entity()
{
    spriteData.width = shipNS::WIDTH;           // size of player
    spriteData.height = shipNS::HEIGHT;
	spriteData.scale = shipNS::SCALING;
    spriteData.x = shipNS::X;                   // location on screen
    spriteData.y = shipNS::Y;
    spriteData.rect.bottom = shipNS::HEIGHT;    // rectangle to select parts of an image
    spriteData.rect.right = shipNS::WIDTH;
    velocity.x = 0;                             // velocity X
    velocity.y = 0;                             // velocity Y
    startFrame = shipNS::player_START_FRAME;     // first frame of ship animation
    endFrame     = shipNS::player_END_FRAME;     // last frame of ship animation
    currentFrame = startFrame;
    radius = shipNS::WIDTH/2.0;
    mass = shipNS::MASS;
    collisionType = entityNS::CIRCLE;
	health = 3;
}

//=============================================================================
// Initialize the Ship.
// Post: returns true if successful, false if failed
//=============================================================================
bool Ship::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM) {
    return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// draw the ship
//=============================================================================
void Ship::draw() {
    Image::draw();              // draw ship
}

//=============================================================================
// update
// typically called once per frame
// deltaTime is used to regulate the speed of movement and animation
//=============================================================================
void Ship::update(float deltaTime)
{
    Entity::update(deltaTime);
    spriteData.angle += deltaTime * shipNS::ROTATION_RATE;  // rotate the ship
    spriteData.x += deltaTime * velocity.x;         // move ship along X 
    spriteData.y += deltaTime * velocity.y;         // move ship along Y

    // Bounce off walls
    if (spriteData.x > GAME_WIDTH-shipNS::WIDTH)    // if hit right screen edge
    {
        spriteData.x = GAME_WIDTH-shipNS::WIDTH;    // position at right screen edge
        velocity.x = -velocity.x;                   // reverse X direction
    } else if (spriteData.x < 0)                    // else if hit left screen edge
    {
        spriteData.x = 0;                           // position at left screen edge
        velocity.x = -velocity.x;                   // reverse X direction
    }
    if (spriteData.y > GAME_HEIGHT-shipNS::HEIGHT)  // if hit bottom screen edge
    {
        spriteData.y = GAME_HEIGHT-shipNS::HEIGHT;  // position at bottom screen edge
        velocity.y = -velocity.y;                   // reverse Y direction
    } else if (spriteData.y < 0)                    // else if hit top screen edge
    {
        spriteData.y = 0;                           // position at top screen edge
        velocity.y = -velocity.y;                   // reverse Y direction
    }
}

//=============================================================================
// damage
//=============================================================================
void Ship::damage(WEAPON weapon) {

	switch (weapon)
	{
		case TORPEDO:
			break;
		case SHIP:
			break;
		case PLANET:
			break;
		case BLACKHOLE: {
							this->setHealth(0);
		} break;
		default:
			break;
	}

}

