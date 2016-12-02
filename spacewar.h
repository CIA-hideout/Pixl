// Programming 2D Games
// Copyright (c) 2011 by: 
// Charles Kelly
// Chapter 6 spacewar.h v1.0

#ifndef _SPACEWAR_H             // Prevent multiple definitions if this 
#define _SPACEWAR_H             // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <map>

#include "game.h"
#include "textureManager.h"
#include "image.h"
#include "ship.h"
#include "Font.h"
#include "Triangle.h"
#include "Circle.h"
#include "Blackhole.h"
#include "Pickup.h"

//=============================================================================
// This class is the core of the game
//=============================================================================
class Spacewar : public Game
{
private:

    // game items
	TextureManager			shipTextures;
	TextureManager			p_deathTextures;
	TextureManager			p_damagedTextures;
	
	TextureManager			triangleTextures;
	TextureManager			circleTextures;
	
	TextureManager			blackHoleTexture;
	TextureManager			obstructorTexture;
	TextureManager			destructorTexture;

	TextureManager			fontTexture;
	TextureManager			heartTexture;

	std::vector<Entity*>	entities;
	std::vector<Entity*>	hearts;

	Ship*					player;
	Blackhole*				blackhole;
	Font*					timeFont;
	Font*					comboFont;
	Font*					scoreFont;

public:
    // Constructor
    Spacewar();

    // Destructor
    virtual ~Spacewar();

    void initialize(HWND hwnd);			// Initialize the game
    void update();						// must override pure virtual from Game
    void ai();							// "
    void collisions();					// "
	void collisions(Entity* entity);	// "
    void render();						// "
    void releaseAll();
    void resetAll();

	void addEntity(Entity* entity);
	void UpdateEntities();
	void DrawEntities();
	int genScore(int combo);								// return Score based on combo
	double calculateF(Entity* entity, Entity* entity2);		// return Force based 2 entities

	bool isEntityAlive(Entity* entity);										// check if an entity has any health left and returns a boolean
	void setPlayerInvulnerable(Entity* entity, float invulTime);			// set invulnerable boolean as true and animate player sprite
};

#endif
