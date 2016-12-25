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
#include "Triangle.h"
#include "Circle.h"
#include "Missile.h"
#include "Pickup.h"
#include "Blackhole.h"
#include "Explosion.h"
#include "Freeze.h"
#include "Font.h"
#include "Effects.h"

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

	// Enemy
	TextureManager			triangleTextures;
	TextureManager			circleTextures;

	// Pickups
	TextureManager			destructorObstructorTexture;
	TextureManager			missileTexture;
	TextureManager			explosionTexture;
	TextureManager			blackHoleTexture;
	TextureManager			freezeTexture;

	// GUI
	TextureManager			fontTexture;
	TextureManager			heartTexture;
	std::vector<Entity*>	hearts;

	// vectors to store the entities
	// entities includes most thing in the game, such as dynamically
	// generated enemies that does not deserve a member variable in
	// this class
	std::vector<Entity*>	entities;
	// missiles is purely to store the missiles. It is seperated from
	// entities to lessen the amount of things to be iterated through.
	std::vector<Missile*>	missiles;

	// Fonts
	Font*					timeFont;
	Font*					comboFont;
	Font*					scoreFont;
	Font*					menuFont;
	Font*					effectFont;

	// since we know that there will be one pickup dedicated to health,
	// it will be easier to store the pointer here for easy referencing
	// the same applies for player
	Pickup*					healthPickup;
	Ship*					player;

	// state of the game
	GameState				gameState;
	// controls the game state
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

	void addEntity(Entity* entity);							// add entities to the entities vector
	void UpdateEntities();
	void DrawEntities();
	int genScore(int combo);								// return Score based on combo
	double calculateF(Entity* entity, Entity* entity2);		// return Force based 2 entities

	GameState getGameState() { return this->gameState; }
	void setGameState(GameState gameState) { this->gameState = gameState; }

	// remove entites from the entities vector to prevent unncessary overhead of iterating dead stuff
	void KillEntities();
};

#endif
