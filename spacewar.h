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
#include "Triangle.h"
#include "Blackhole.h"

//=============================================================================
// This class is the core of the game
//=============================================================================
class Spacewar : public Game
{
private:

    // game items
	TextureManager			shipTextures;
	TextureManager			p_deathTextures;
	TextureManager			triangleTextures;
	TextureManager			blackHoleTexture;
	TextureManager			fontTexture;

	std::vector<Entity*>	entities;

	Ship*					player;
	Blackhole*				blackhole;

public:
    // Constructor
    Spacewar();

    // Destructor
    virtual ~Spacewar();

    // Initialize the game
    void initialize(HWND hwnd);
    void update();      // must override pure virtual from Game
    void ai();          // "
    void collisions();  // "
    void render();      // "
    void releaseAll();
    void resetAll();

	void addEntity(Entity* entity);
	void UpdateEntities();
	void DrawEntities();
	int genScore(int combo);								// return Score based on combo
	double calculateF(Entity* entity, Entity* entity2);		// return Force based 2 entities
};

#endif
