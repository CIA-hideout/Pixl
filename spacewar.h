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

enum GameState {
	GAME_STATE_MENU,
	GAME_STATE_GAME,
	GAME_STATE_SETTING
};

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
	TextureManager			circleTextures;
	TextureManager			blackHoleTexture;
	TextureManager			fontTexture;
	TextureManager			heartTexture;
	TextureManager			destructorTexture;
	TextureManager			obstructorTexture;

	std::vector<Entity*>	entities;
	std::vector<Entity*>	hearts;

	Ship*					player;
	Blackhole*				blackhole;
	Font*					timeFont;
	Font*					comboFont;
	Font*					scoreFont;
	Font*					menuFont;

	Pickup*					generalPickup;
	Pickup*					healthPickup;

	GameState				gameState;
	bool					isRunning;

public:
	// Constructor
	Spacewar();

	// Destructor
	virtual ~Spacewar();

	void initialize(HWND hwnd);
	void update();
	void ai();
	void collisions();
	void collisions(Entity* entity);
	void render();
	void releaseAll();
	void resetAll();

	void addEntity(Entity* entity);
	void UpdateEntities();
	void DrawEntities();
	int genScore(int combo);								// return Score based on combo
	double calculateF(Entity* entity, Entity* entity2);		// return Force based 2 entities

	bool isEntityAlive(Entity* entity);						// check if an entity has any health left and returns a boolean

	GameState getGameState() { return this->gameState; }
	void setGameState(GameState gameState) { this->gameState = gameState; }

	bool getIsRunning() { return this->isRunning; }
	void setIsRunning(bool isRunning) { this->isRunning; }
};

#endif
