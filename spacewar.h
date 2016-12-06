// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

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
	GAME_STATE_SETTING,
	GAME_STATE_GAMEOVER
};

class Spacewar : public Game {
private:

	// game items
	// Player
	TextureManager			shipTextures;
	TextureManager			p_deathTextures;
	TextureManager			p_invulTextures;
	TextureManager			p_invinTextures;

	// Enemy
	TextureManager			triangleTextures;
	TextureManager			circleTextures;

	// Pickups
	TextureManager			destructorObstructorTexture;
	TextureManager			missileTexture;
	TextureManager			explosionTexture;
	TextureManager			blackHoleTexture;

	// GUI
	TextureManager			fontTexture;
	TextureManager			heartTexture;

	std::vector<Entity*>	entities;
	std::vector<Entity*>	hearts;
	std::vector<Missile*>	missiles;

	Ship*					player;
	Font*					timeFont;
	Font*					comboFont;
	Font*					scoreFont;
	Font*					menuFont;
	Font*					effectFont;

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

	GameState getGameState() { return this->gameState; }
	void setGameState(GameState gameState) { this->gameState = gameState; }

	bool getIsRunning() { return this->isRunning; }
	void setIsRunning(bool isRunning) { this->isRunning; }

	void KillEntities();
};

#endif
