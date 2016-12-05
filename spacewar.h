// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly Go And Fuck Yourself
// Chapter 6 spacewar.h v1.0

#ifndef _SPACEWAR_H
#define _SPACEWAR_H
#define WIN32_LEAN_AND_MEAN

#include "game.h"
#include "textureManager.h"
#include "image.h"
#include "ship.h"
#include "Font.h"
#include "Triangle.h"
#include "Circle.h"
#include "Blackhole.h"
#include "Missile.h"
#include "Pickup.h"
#include "Explosion.h"

enum GameState {
	GAME_STATE_MENU,
	GAME_STATE_GAME,
	GAME_STATE_SETTING
};

class Spacewar : public Game {
private:

	// game items
	TextureManager			shipTextures;
	TextureManager			p_deathTextures;
	TextureManager			p_damagedTextures;

	TextureManager			triangleTextures;
	TextureManager			circleTextures;

	TextureManager			obstructorTexture;
	TextureManager			destructorTexture;
	TextureManager			destructorObstructorTexture;
	TextureManager			missileTexture;
	TextureManager			explosionTexture;
	TextureManager			blackHoleTexture;

	TextureManager			fontTexture;
	TextureManager			heartTexture;

	std::vector<Entity*>	entities;
	std::vector<Entity*>	hearts;
	std::vector<Missile*>	missiles;

	Ship*					player;
	Blackhole*				blackhole;
	Font*					timeFont;
	Font*					comboFont;
	Font*					scoreFont;
	Font*					menuFont;
	Font*					effectFont;

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

	bool isEntityAlive(Entity* entity);										// check if an entity has any health left and returns a boolean
	void setPlayerInvulnerable(Entity* entity, float invulTime);			// set invulnerable boolean as true and animate player sprite

	GameState getGameState() { return this->gameState; }
	void setGameState(GameState gameState) { this->gameState = gameState; }

	bool getIsRunning() { return this->isRunning; }
	void setIsRunning(bool isRunning) { this->isRunning; }

	void KillEntities();
};
#endif
