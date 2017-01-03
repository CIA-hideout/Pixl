// Module:			Gameplay Programming
// Assignment 1:	Pixl
// Student Name:	Elcoms Khang	(S10157558A)
//					Lin Lue			(S10158175E)
//					Amos Tan		(S10158017D)

#include "spaceWar.h"

// calculates the number of enemies
#define TRIANGLE_COUNT(x) (x * 0.15 * 16)
#define CIRCLE_COUNT(x) (x * 0.15 * 8)

// just a few helper functions that does not deserve to be in a class
void PrintEffect(Entity* entity, Font* effectFont);					// prints the effect that the player has
bool isWaveOver(std::vector<Entity*> entities);						// function to check if the wave is over based on
bool isTargeted(std::vector<Missile*> missiles, Entity* entity);	// checks if an entitiy is targeted by a missile
int minMaxRand(int min, int max);									// generate a random integer based on minimum/maximum value

// values that will be used constantly so a might as well make them global

float	playerAccelerationRate;										// Used to speed up player's velocity
float	playerDeccelerationRate;									// Used to slow down player's velocity
float	playerTurnMultiplier;										// Controls how fast the player can rotate
float	slowedTime, slowRadians;									// Used for slowing down blackhole rotation
float	deathAngle;													// the angle in radians at the point in time of the player's death
float	waveBufferTime;												// a short peroid of time before the first wave and menu
float	blackholeTimer;												// time where the blackhole is left in the entities vector

int		playerMaxHealth, playerHealth;
int		playerScore, highscore;										// highscore will be written in a file when it is greater than the current highscore
int		combo, maxCombo;											// record combo
int		currentWave;

bool	waveOver;
bool	playerIsDead, playerCanPickup;
bool	playerDefaultTexture;										// true/false based on whether player has their starting texture or changed
bool	beatenHighScore;
bool	isKeyPressed;													// true/false based on whether player is on the how to play instructions page

int		cursorPos, cursorPosCount;									// a number from 0 - 31
int		kbdStartPosX, kbdStartPosY = 50;

DWORD	baseTime;

std::stringstream ss;
std::string scoreStr;

Spacewar::Spacewar() {}

Spacewar::~Spacewar() {
	releaseAll();           // call onLostDevice() for every graphics item
}

void Spacewar::initialize(HWND hwnd) {
	srand(timeGetTime());

	Game::initialize(hwnd);

	//AllocConsole();		// Console for debugging
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	//=================================================
	//				Texture Initialization
	//=================================================
	// Player
	shipTextures.initialize(graphics, PLAYER_TEXTURE);
	p_deathTextures.initialize(graphics, PLAYER_DEATH_TEXTURE);
	p_invulTextures.initialize(graphics, PLAYER_INVUL_TEXTURE);
	p_invinTextures.initialize(graphics, PLAYER_INVIN_TEXTURE);

	// Enemy
	triangleTextures.initialize(graphics, TRIANGLE_TEXTURE);
	circleTextures.initialize(graphics, CIRCLE_TEXTURE);
	blackHoleTexture.initialize(graphics, BLACKHOLE_TEXTURE);

	// Pickups
	missileTexture.initialize(graphics, MISSILE_TEXTURE);
	destructorObstructorTexture.initialize(graphics, DESTRUCTOR_OBSTRUCTOR_TEXTURE);
	explosionTexture.initialize(graphics, EXPLOSION_TEXTURE);
	freezeTexture.initialize(graphics, FREEZE_TEXTURE);

	// GUI
	heartTexture.initialize(graphics, HEART_TEXTURE);
	fontTexture.initialize(graphics, FONT_TEXTURE);
	instructionsTexture.initialize(graphics, INSTRUCTIONS_TEXTURE);

	instructions = new Entity();
	instructions->initialize(this, INSTRUCTIONS_WIDTH, INSTRUCTIONS_HEIGHT, INSTRUCTIONS_COLS, &instructionsTexture);
	instructions->setLoop(false);
	instructions->setFrames(INSTRUCTIONS_CONTROLS_START_FRAME, INSTRUCTIONS_CONTROLS_END_FRAME);
	instructions->setCurrentFrame(INSTRUCTIONS_CONTROLS_START_FRAME);
	instructions->setX(GAME_WIDTH / 2 - instructions->getScale() * instructions->getWidth() / 2);
	instructions->setY(GAME_HEIGHT / 2 - instructions->getScale() * instructions->getHeight() / 2);
	bool isKeyPressed = false;

	this->setGameState(GAME_STATE_MENU);

	//=================================================
	//				Font Initialization
	//=================================================
	timeFont = new Font();
	timeFont->initialize(this, 2048, 2048, 16, &fontTexture);
	timeFont->loadTextData(FONT_TEXTURE_INFO);
	timeFont->setHeight(128);
	timeFont->setWidth(128);

	comboFont = new Font();
	comboFont->initialize(this, 2048, 2048, 16, &fontTexture);
	comboFont->loadTextData(FONT_TEXTURE_INFO);
	comboFont->setHeight(128);
	comboFont->setWidth(128);

	scoreFont = new Font();
	scoreFont->initialize(this, 2048, 2048, 16, &fontTexture);
	scoreFont->loadTextData(FONT_TEXTURE_INFO);
	scoreFont->setHeight(128);
	scoreFont->setWidth(128);

	menuFont = new Font();
	menuFont->initialize(this, 2048, 2048, 16, &fontTexture);
	menuFont->loadTextData(FONT_TEXTURE_INFO);
	menuFont->setHeight(128);
	menuFont->setWidth(128);

	effectFont = new Font();
	effectFont->initialize(this, 2048, 2048, 16, &fontTexture);
	effectFont->loadTextData(FONT_TEXTURE_INFO);
	effectFont->setHeight(128);
	effectFont->setWidth(128);
	effectFont->setScale(0.3);

	// positioning of hearts
	int dx = GAME_WIDTH - GAME_WIDTH / 100;
	for (int i = 0; i < 10; i++) {
		Entity* heart = new Entity();
		heart->initialize(this, 128, 128, 2, &heartTexture);
		heart->setCurrentFrame(0);
		heart->setVelocity(0, 0);
		heart->setMass(0);
		heart->setRadians(0);
		heart->setScale(0.4);
		heart->setX(dx - heart->getWidth() * heart->getScale());
		heart->setY(GAME_HEIGHT - heart->getHeight() * heart->getScale() - 10);
		dx -= heart->getWidth() * heart->getScale();
		hearts.push_back(heart);
	}

	ParseScore(HIGHSCORE_FILE);

	for (std::map<int, std::string>::iterator iter = this->highscores.begin(); iter != this->highscores.end(); iter++) {
		scoreVect.push_back(iter->first);
	}
	std::sort(scoreVect.begin(), scoreVect.end(), std::greater<int>());

	inputMap.insert(std::pair<UCHAR, bool>(VK_LEFT, false));
	inputMap.insert(std::pair<UCHAR, bool>(VK_RIGHT, false));
	inputMap.insert(std::pair<UCHAR, bool>(VK_UP, false));
	inputMap.insert(std::pair<UCHAR, bool>(VK_DOWN, false));
	inputMap.insert(std::pair<UCHAR, bool>(VK_ESCAPE, false));
	inputMap.insert(std::pair<UCHAR, bool>(VK_RETURN, false));
	inputMap.insert(std::pair<UCHAR, bool>(0x5A, false));		// z
	inputMap.insert(std::pair<UCHAR, bool>(0x48, false));		// h
	inputMap.insert(std::pair<UCHAR, bool>(0x49, false));		// i
	inputMap.insert(std::pair<UCHAR, bool>(0x43, false));		// c

	kbdStartPosX = GAME_WIDTH / 2 - menuFont->getTotalWidth_("ABCDEFGH") / 2;
	kbdStartPosY = GAME_HEIGHT / 2;

	TextureManager* selectBoxTexture = new TextureManager();
	selectBoxTexture->initialize(graphics, SELECT_BOX_TEXTURE);
	selectBox = new Entity();
	selectBox->initialize(this, 128, 128, 1, selectBoxTexture);
	selectBox->setScale(0.5f);
	selectBox->setX(kbdStartPosX);
	selectBox->setY(kbdStartPosY);

	alphaVect = std::vector<int>();

	for (int i = 'A'; i <= '`'; i++) {
		alphaVect.push_back(i);
	}

	textVect = std::vector<int>();

	return;
}

void Spacewar::update() {
	//for (std::map<UCHAR, bool>::iterator iter = inputMap.begin(); iter != inputMap.end(); iter++) {
	//	if (input->isKeyDown(iter->first)) {
	//		iter->second = true;
	//	}
	//	else {
	//		iter->second = false;
	//	}
	//}

	switch (this->getGameState()) {
	case GAME_STATE_MENU: {
							  //=================================================
							  //					   MENU
							  //=================================================
							  // Code to run in the main menu
							  // Start the game state if the spacebar is pressed
							  // Variables that may have be changed in the previous state(s) will be set to 0/original values again

							  // reset instructions to page 1
							  instructions->setFrames(INSTRUCTIONS_CONTROLS_START_FRAME, INSTRUCTIONS_CONTROLS_END_FRAME);
							  instructions->setCurrentFrame(INSTRUCTIONS_CONTROLS_START_FRAME);
							  instructions->setLoop(false);

							  // s
							  if (input->isKeyDown(0x53)) {
								  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);

								  entities.clear();

								  beatenHighScore = false;
								  playerHealth = 5;
								  playerMaxHealth = 10;
								  playerIsDead = false;
								  playerDefaultTexture = true;
								  combo = maxCombo = playerScore = 0;

								  this->setGameState(GAME_STATE_GAME);

								  playerAccelerationRate = 3.5f;
								  playerDeccelerationRate = 0.005f;
								  playerTurnMultiplier = 3.5f;

								  //=================================================
								  //			 Entities Initialization
								  //=================================================
								  // initializes default values and add them into entites vector
								  // Player
								  player = new Ship();
								  player->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
								  player->setCurrentFrame(shipNS::player_START_FRAME);
								  player->setObjectType(OBJECT_TYPE_PLAYER);
								  player->setRadians(0);
								  player->setVisible(true);
								  player->setX(GAME_WIDTH / 2 - player->getWidth() / 2 * player->getScale());
								  player->setY(GAME_HEIGHT / 2 - player->getHeight() / 2 * player->getScale());
								  player->setHealth(playerHealth);

								  this->addEntity(player);

								  // Health Pickup
								  /*healthPickup = new Pickup();
								  healthPickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &heartTexture);
								  healthPickup->setPickUpType(PICKUP_HEALTH);
								  healthPickup->setCurrentFrame(0);
								  healthPickup->setNewLocation();

								  addEntity(healthPickup);*/

								  // Other Pickups (Obstructor/Destructors)
								  for (int i = 0; i < 3; i++) {
									  Pickup* pickup = new Pickup();
									  pickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &destructorObstructorTexture);
									  pickup->calculateObstructorDestructorType();
									  if (pickup->getIsDestructor())
										  pickup->setCurrentFrame(0);
									  else
										  pickup->setCurrentFrame(1);
									  pickup->setNewLocation();

									  addEntity(pickup);
								  }

								  // Enemies (Initialize first wave)
								  currentWave = 1;
								  for (int i = 0; i < TRIANGLE_COUNT(currentWave); i++) {
									  Triangle* triangle = new Triangle();
									  triangle->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
									  triangle->setObjectType(OBJECT_TYPE_TRIANGLE);
									  triangle->setActive(true);
									  triangle->setHealth(1);
									  triangle->spawn();
									  addEntity(triangle);
								  }
								  for (int i = 0; i <= CIRCLE_COUNT(currentWave); i++) {
									  Circle* circle = new Circle();
									  circle->initialize(this, CircleNS::WIDTH, CircleNS::HEIGHT, CircleNS::TEXTURE_COLS, &circleTextures);
									  circle->setObjectType(OBJECT_TYPE_TRIANGLE);
									  circle->setHealth(1);
									  circle->spawn();
									  addEntity(circle);
								  }
								  waveBufferTime = 1.5f;			// time pause in between new waves
								  baseTime = timeGetTime();
							  }

							  // c
							  else if (input->isKeyDown(0x43)) {
								  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
								  this->setGameState(GAME_STATE_CREDITS);
							  }

							  // i
							  else if (input->isKeyDown(0x49)) {
								  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
								  this->setGameState(GAME_STATE_INSTRUCTIONS);
							  }

							  // h
							  else if (input->isKeyDown(0x48)) {
								  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
								  this->setGameState(GAME_STATE_HIGHSCORE);
								  cursorPos = 0;
							  }
	} break;
	case GAME_STATE_GAME: {
							  //=================================================
							  //					GAMEPLAY
							  //=================================================
							  // Code to run when gameplay is ongoing

							  // buffer time for the first wave
							  if (waveBufferTime > 0.0f) {
								  waveBufferTime -= deltaTime;
							  }

							  // update health bar based on player's current health (increment/decrement)
							  for (int i = 0; i < hearts.size(); i++) {
								  hearts[i]->setCurrentFrame(1);
								  hearts[i]->update(deltaTime);
							  }
							  for (int i = 0; i < hearts.size() - player->getHealth(); i++) {
								  hearts[i]->setCurrentFrame(0);
								  hearts[i]->update(deltaTime);
							  }

							  // check if the current wave is over; spawns new stuff if true
							  if (isWaveOver(entities)) {
								  currentWave++;
								  waveBufferTime = 1.5f;
								  for (int i = 0; i < TRIANGLE_COUNT(currentWave); i++) {
									  Triangle* tri = new Triangle();
									  tri->initialize(this, TriangleNS::WIDTH, TriangleNS::HEIGHT, TriangleNS::TEXTURE_COLS, &triangleTextures);
									  tri->setObjectType(OBJECT_TYPE_TRIANGLE);
									  tri->setActive(true);
									  tri->setHealth(1);
									  tri->spawn();
									  addEntity(tri);
								  }
								  for (int i = 0; i <= CIRCLE_COUNT(currentWave); i++) {
									  Circle* circle = new Circle();
									  circle->initialize(this, CircleNS::WIDTH, CircleNS::HEIGHT, CircleNS::TEXTURE_COLS, &circleTextures);
									  circle->setObjectType(OBJECT_TYPE_TRIANGLE);
									  circle->setHealth(1);
									  circle->spawn();
									  addEntity(circle);
								  }
							  }

							  // checks the collision of missiles and entites here
							  VECTOR2 collisionVector;
							  for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
								  (*iter)->update(deltaTime);
								  // Re-targets a new enemy if the previous target dies before the missile collides with it
								  if ((*iter)->getTarget()->getActive()) {
									  if ((*iter)->getObjectType() == OBJECT_TYPE_MISSILE) {
										  if ((*iter)->collidesWith(*(*iter)->getTarget(), collisionVector)) {
											  switch ((*iter)->getTarget()->getObjectType()) {
											  case OBJECT_TYPE_TRIANGLE: {
																			 Triangle* triangle = (Triangle*)((*iter)->getTarget());
																			 triangle->damage(WEAPON_MISSILE);
																			 (*iter)->setVisible(false);
																			 (*iter)->setActive(false);
											  } break;
											  case OBJECT_TYPE_CIRCLE: {
																		   Circle* circle = (Circle*)((*iter)->getTarget());
																		   circle->damage(WEAPON_MISSILE);
																		   (*iter)->setVisible(false);
																		   (*iter)->setActive(false);
											  } break;
											  case OBJECT_TYPE_BOSS: {
											  } break;
											  }
										  }
									  }
								  }
								  // Find a new target here, target should not be targeted by another missile
								  else {
									  for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
										  if (
											  ((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
											  (*iter_)->getActive() == true &&
											  !isTargeted(missiles, *iter_)
											  ) {
											  (*iter)->setTarget(*iter_);
										  }
									  }
									  // kills itself if no target is available
									  if (!(*iter)->getTarget()->getActive()) {
										  (*iter)->setActive(false);
									  }
								  }
							  }

							  // explosion detection
							  for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
								  if ((*iter)->getObjectType() == OBJECT_TYPE_EXPLOSION) {
									  for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
										  if (((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
											  (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
											  (*iter_)->getActive() == true) {
											  if ((*iter_)->collidesWith(**iter, collisionVector)) {
												  switch ((*iter_)->getObjectType()) {
												  case OBJECT_TYPE_TRIANGLE: {
																				 Triangle* triangle = (Triangle*)(*iter_);
																				 triangle->damage(WEAPON_EXPLOSION);
												  } break;
												  case OBJECT_TYPE_CIRCLE: {
																			   Circle* circle = (Circle*)(*iter_);
																			   circle->damage(WEAPON_EXPLOSION);
												  } break;
												  case OBJECT_TYPE_BOSS: {
												  } break;
												  }
											  }
										  }
									  }
								  }
							  }

							  // update combo as the game progresses
							  if (combo > maxCombo)
								  maxCombo = combo;

							  // Pressing ESC Key returns player to Main Menu
							  if (input->isKeyDown(ESC_KEY)) {
								  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);

								  this->setGameState(GAME_STATE_MENU);
							  }

							  // cleanup entities with no health
							  KillEntities();
	} break;
	case GAME_STATE_SETTING: {
	} break;
	case GAME_STATE_GAMEOVER: {
								  //=================================================
								  //					END SCREEN
								  //=================================================
								  // Code to run after the player dies and the score is shown
								  // Press ESC Key to return to Main Menu
								  if (input->isKeyDown(ESC_KEY)) {
									  PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);

									  this->setGameState(GAME_STATE_MENU);
								  }

								  // only compare with the top 10

								  if (scoreVect.size() < 10) {
									  this->setGameState(GAME_STATE_NEW_HIGHSCORE);
								  }
								  else {
									  for (int i = 0; i < scoreVect.size(); i++) {
										  if (playerScore > scoreVect[i] && playerScore > 0) {
											  this->setGameState(GAME_STATE_NEW_HIGHSCORE);
										  }
									  }
								  }
	} break;
	case GAME_STATE_CREDITS: {
								 if (input->isKeyDown(VK_ESCAPE)) {
									 PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
									 this->setGameState(GAME_STATE_MENU);
								 }
	} break;
	case GAME_STATE_INSTRUCTIONS: {
		if (input->isKeyDown(VK_ESCAPE)) {
			PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
			this->setGameState(GAME_STATE_MENU);
		}
		else if (instructions->getCurrentFrame() == 0 && input->isKeyDown(VK_RIGHT) && !isKeyPressed)
		{
			PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
			instructions->setFrames(INSTRUCTIONS_HOW_START_FRAME, INSTRUCTIONS_HOW_END_FRAME);
			instructions->setCurrentFrame(INSTRUCTIONS_HOW_START_FRAME);
			instructions->setLoop(false);
			isKeyPressed = true;
		}
		else if (instructions->getCurrentFrame() == 1 && input->isKeyDown(VK_LEFT) && !isKeyPressed)
		{
			PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
			instructions->setFrames(INSTRUCTIONS_CONTROLS_START_FRAME, INSTRUCTIONS_CONTROLS_END_FRAME);
			instructions->setCurrentFrame(INSTRUCTIONS_CONTROLS_START_FRAME);
			instructions->setLoop(false);
			isKeyPressed = true;
		}
		else if (instructions->getCurrentFrame() == 1 && input->isKeyDown(VK_RIGHT) && !isKeyPressed)
		{
			PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
			instructions->setFrames(INSTRUCTIONS_GOODLUCK_START_FRAME, INSTRUCTIONS_GOODLUCK_END_FRAME);
			instructions->setCurrentFrame(INSTRUCTIONS_GOODLUCK_START_FRAME);
			instructions->setFrameDelay(INSTRUCTIONS_ANIMATION_DELAY);
			instructions->setLoop(true);
			isKeyPressed = true;
		}
		else if (instructions->getCurrentFrame() == 2 && input->isKeyDown(VK_LEFT) && !isKeyPressed)
		{
			PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
			instructions->setFrames(INSTRUCTIONS_HOW_START_FRAME, INSTRUCTIONS_HOW_END_FRAME);
			instructions->setCurrentFrame(INSTRUCTIONS_HOW_START_FRAME);
			instructions->setLoop(false);
			isKeyPressed = true;
		}

		if (timeGetTime() % 1000 <= 50)
		{
			isKeyPressed = false;
		}

		instructions->update(deltaTime);
	} break;
	case GAME_STATE_HIGHSCORE: {
								   if (input->isKeyDown(VK_ESCAPE)) {
									   PlaySound(PLAYER_SELECT_SOUND, NULL, SND_ASYNC);
									   this->setGameState(GAME_STATE_MENU);
								   }
	} break;
	case GAME_STATE_NEW_HIGHSCORE:{
									  // r
									  if (input->isKeyDown(VK_RIGHT)) {
										  if (inputMap.at(VK_RIGHT) == false) {
											  cursorPos++;
											  inputMap.at(VK_RIGHT) = true;
										  }
									  }
									  else {
										  inputMap.at(VK_RIGHT) = false;
									  }

									  // l
									  if (input->isKeyDown(VK_LEFT)) {
										  if (inputMap.at(VK_LEFT) == false) {
											  cursorPos--;
											  inputMap.at(VK_LEFT) = true;
										  }
									  }
									  else {
										  inputMap.at(VK_LEFT) = false;
									  }

									  // u
									  if (input->isKeyDown(VK_UP)) {
										  if (inputMap.at(VK_UP) == false) {
											  cursorPos -= 8;
											  inputMap.at(VK_UP) = true;
										  }
									  }
									  else {
										  inputMap.at(VK_UP) = false;
									  }

									  // d
									  if (input->isKeyDown(VK_DOWN)) {
										  if (inputMap.at(VK_DOWN) == false) {
											  cursorPos += 8;
											  inputMap.at(VK_DOWN) = true;
										  }
									  }
									  else {
										  inputMap.at(VK_DOWN) = false;
									  }

									  if (cursorPos < 0) {
										  cursorPos = cursorPos + 32;
									  }

									  cursorPos %= 32;

									  // select a character at the position of the cursor
									  if (input->isKeyDown(0x5A)) {
										  if (inputMap.at(0x5A) == false) {
											  // the current selected item will be added
											  // backspace
											  if (alphaVect[cursorPos] == (int)'_') {
												  if (!textVect.empty()) {
													  textVect.pop_back();
												  }
											  }
											  // return
											  else if (alphaVect[cursorPos] == (int)'`') {
												  if (textVect.size() == 3) {
													  std::string scoreStr_(textVect.begin(), textVect.end());
													  highscores.insert(std::pair<int, std::string>(playerScore, scoreStr_));

													  highscores.insert(std::pair<int, std::string>(playerScore, scoreStr_));
													  scoreVect.push_back(playerScore);

													  std::ofstream ofile;
													  ofile.open(HIGHSCORE_FILE, std::ios_base::app);
													  ofile << "\n" << scoreStr_ << ";" << playerScore;
													  ofile.close();

													  textVect.clear();
													  setGameState(GAME_STATE_HIGHSCORE);
												  }
											  }
											  else {
												  if (textVect.size() < 3)
													  textVect.push_back(alphaVect[cursorPos]);
											  }
											  inputMap.at(0x5A) = true;
										  }
									  }
									  else {
										  inputMap.at(0x5A) = false;
									  }

									  printf("cursor pos: %d\n", cursorPos);

									  selectBox->setX(kbdStartPosX + cursorPos % 8 * 128 * menuFont->getScale());
									  selectBox->setY(kbdStartPosY + ((cursorPos - (cursorPos % 8)) / 8) % 4 * 128 * menuFont->getScale());
	};
	}
}

void Spacewar::ai() {}

void Spacewar::render() {
	graphics->spriteBegin();

	switch (this->getGameState()) {
	case GAME_STATE_MENU: {
							  //=================================================
							  //					  MENU
							  //=================================================
							  // displays GUI for the main menu
							  menuFont->Print(
								  GAME_WIDTH / 2 - menuFont->getTotalWidth("P i x l .") / 2,
								  GAME_HEIGHT / 3,
								  "P i x l .");
							  //menuFont->Print(
							  // GAME_WIDTH / 2 - menuFont->getTotalWidth("[S]tart") / 2,
							  // GAME_HEIGHT / 2, "[S]tart");

							  int offsetY = 0;
							  for (int i = 0; i < MENU_OPTIONS.size(); i++) {
								  menuFont->Print(GAME_WIDTH / 2 - menuFont->getTotalWidth(MENU_OPTIONS[i]) / 2, GAME_HEIGHT / 2 + offsetY, MENU_OPTIONS[i]);
								  offsetY += menuFont->getHeight() * menuFont->getScale();
							  }
	} break;
	case GAME_STATE_CREDITS: {
								 int offsetY = 0;
								 menuFont->Print(GAME_WIDTH / 2 - menuFont->getTotalWidth("CREDITS:") / 2, GAME_HEIGHT / 4, "Credits:");
								 for (int i = 0; i < CREDITS.size(); i++) {
									 menuFont->Print(GAME_WIDTH / 2 - menuFont->getTotalWidth(CREDITS[i]) / 2, GAME_HEIGHT / 3 + offsetY, CREDITS[i]);
									 offsetY += menuFont->getHeight() * menuFont->getScale();
								 }
	} break;
	case GAME_STATE_INSTRUCTIONS: {
									 instructions->draw();
	} break;
	case GAME_STATE_GAME: {
							  //=================================================
							  //					GAMEPLAY
							  //=================================================
							  // Code to draw textures for entites and GUI during gameplay
							  if (waveBufferTime > 0.0f) {
								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth("wave" + std::to_string(currentWave)) / 2,
									  GAME_HEIGHT / 3,
									  "wave " + std::to_string(currentWave));
								  // buffer will not display anything except for wave number for the first wave
								  if (currentWave != 1) {
									  DrawEntities();
									  ss.str("");
									  ss << std::fixed << std::setprecision(1) << (float)(timeGetTime() - baseTime) / 1000;
									  timeFont->Print(GAME_WIDTH / 2 - timeFont->getTotalWidth(ss.str()) / 2, 10, ss.str());
									  ss.str("x" + std::to_string(combo));
									  comboFont->Print(10, GAME_HEIGHT - comboFont->getHeight() * comboFont->getScale(), ss.str());
									  ss.str(std::to_string(playerScore));
									  scoreFont->Print(10, 10, ss.str());
									  for (std::vector<Entity*>::iterator iter = hearts.begin(); iter != hearts.end(); iter++) {
										  (*iter)->draw();
									  }
									  PrintEffect(player, effectFont);
									  for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
										  (*iter)->draw();
									  }
								  }
							  }
							  else {
								  DrawEntities();
								  ss.str("");
								  ss << std::fixed << std::setprecision(1) << (float)(timeGetTime() - baseTime) / 1000;
								  timeFont->Print(GAME_WIDTH / 2 - timeFont->getTotalWidth(ss.str()) / 2, 10, ss.str());
								  ss.str("x" + std::to_string(combo));
								  comboFont->Print(10, GAME_HEIGHT - comboFont->getHeight() * comboFont->getScale(), ss.str());
								  ss.str(std::to_string(playerScore));
								  scoreFont->Print(10, 10, ss.str());
								  for (std::vector<Entity*>::iterator iter = hearts.begin(); iter != hearts.end(); iter++) {
									  (*iter)->draw();
								  }
								  float dy = 10;
								  PrintEffect(player, effectFont);
								  for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
									  (*iter)->draw();
								  }
							  }
	} break;
	case GAME_STATE_SETTING: {
	} break;
	case GAME_STATE_GAMEOVER: {
								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth("Game over") / 2,
									  GAME_HEIGHT / 3,
									  "Game over"
									  );

								  ss.str("");
								  ss << "score: " << playerScore << " Highest combo: " << maxCombo << " wave: " << currentWave;

								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth(ss.str()) / 2,
									  GAME_HEIGHT / 2.5,
									  ss.str()
									  );
								  menuFont->Print(
									  GAME_WIDTH / 2 - menuFont->getTotalWidth("Press esc to return to main menu") / 2,
									  GAME_HEIGHT / 2, "Press esc to return to main menu");

								  ss.str("");

								  // notifies the player if he has beaten the highscore
								  if (beatenHighScore) {
									  ss << "New highscore!";
									  menuFont->Print(
										  GAME_WIDTH / 2 - menuFont->getTotalWidth(ss.str()) / 2,
										  GAME_HEIGHT / 4,
										  ss.str()
										  );
								  }
								  // Displays the highscore
								  else {
									  ss << "Highscore: " << highscore;
									  menuFont->Print(
										  GAME_WIDTH / 2 - menuFont->getTotalWidth(ss.str()) / 2,
										  GAME_HEIGHT - GAME_HEIGHT / 4,
										  ss.str()
										  );
								  }
	} break;
	case GAME_STATE_HIGHSCORE: {
								   menuFont->Print(
									   GAME_WIDTH / 2 - menuFont->getTotalWidth("Highscores") / 2,
									   GAME_HEIGHT / 7,
									   "Highscores"
									   );

								   ss.str("");

								   int offsetY = GAME_HEIGHT / 5;
								   int colCount = GAME_WIDTH / 8;
								   int colName = GAME_WIDTH / 5;
								   int colScore = GAME_WIDTH - GAME_WIDTH / 8;

								   std::sort(scoreVect.begin(), scoreVect.end(), std::greater<int>());

								   for (int i = 0; i < scoreVect.size() && i < 10; i++) {
									   ss << i + 1;
									   menuFont->Print(colCount, offsetY, ss.str());
									   ss.str("");

									   menuFont->Print(colName, offsetY, highscores.at(scoreVect[i]));

									   ss << scoreVect[i];
									   menuFont->Print(colScore - menuFont->getTotalWidth(ss.str()), offsetY, ss.str());
									   ss.str("");

									   offsetY += menuFont->getScale() * menuFont->getHeight();
								   }

								   menuFont->Print(
									   GAME_WIDTH / 2 - menuFont->getTotalWidth("Press ESC to return to main menu") / 2,
									   GAME_HEIGHT - GAME_HEIGHT / 9,
									   "Press ESC to return to main menu"
									   );

	} break;
	case GAME_STATE_NEW_HIGHSCORE: {
									   ss.str("");
									   ss << "score: " << playerScore << " Highest combo: " << maxCombo << " wave: " << currentWave;

									   menuFont->Print(
										   GAME_WIDTH / 2 - menuFont->getTotalWidth(ss.str()) / 2,
										   GAME_HEIGHT / 5,
										   ss.str()
										   );

									   menuFont->Print_(
										   GAME_WIDTH / 2 - menuFont->getTotalWidth_("New Highscore!") / 2,
										   GAME_HEIGHT / 4,
										   "New Highscore!");

									   menuFont->Print(
										   GAME_WIDTH / 2 - menuFont->getTotalWidth("Press the <z> key to select an alphabet") / 2,
										   GAME_HEIGHT / 2.5,
										   "Press the <z> key to select an alphabet");

									   int alphaOffset = (int)'A';
									   int offsetX = kbdStartPosX;
									   int offsetY = kbdStartPosY;
									   int count = 1;
									   std::stringstream ss;
									   std::string character;
									   for (int i = alphaOffset; i <= (int)'`'; i++) {
										   ss << (char)i;
										   ss >> character;
										   ss.clear();
										   menuFont->Print_(offsetX, offsetY, character);
										   offsetX += menuFont->getTotalWidth_(character);
										   if (count % 8 == 0) {
											   offsetY += menuFont->getHeight() * menuFont->getScale();
											   offsetX = kbdStartPosX;
										   }
										   count++;
									   }
									   // make the last character the '>' key
									   selectBox->draw();

									   std::string scoreStr(textVect.begin(), textVect.end());
									   menuFont->Print_(
										   GAME_WIDTH / 2 - menuFont->getTotalWidth_(scoreStr) / 2,
										   GAME_HEIGHT / 3,
										   scoreStr
										   );
	} break;
	}

	graphics->spriteEnd();
}

// release resources
void Spacewar::releaseAll() {
	shipTextures.onResetDevice();
	circleTextures.onResetDevice();
	p_deathTextures.onResetDevice();
	triangleTextures.onResetDevice();
	circleTextures.onResetDevice();
	blackHoleTexture.onResetDevice();
	fontTexture.onResetDevice();
	heartTexture.onResetDevice();
	Game::releaseAll();
	FreeConsole();
	return;
}

// reset devices
void Spacewar::resetAll() {
	shipTextures.onResetDevice();
	triangleTextures.onResetDevice();
	shipTextures.onResetDevice();
	circleTextures.onResetDevice();
	p_deathTextures.onResetDevice();
	triangleTextures.onResetDevice();
	circleTextures.onResetDevice();
	blackHoleTexture.onResetDevice();
	fontTexture.onResetDevice();
	heartTexture.onResetDevice();
	Game::resetAll();
	FreeConsole();
	return;
}

// adds entity. static counter applies id
void Spacewar::addEntity(Entity* entity) {
	static int id_counter = 0;
	entity->setObjectId(id_counter);
	this->entities.push_back(entity);
	id_counter++;
}

void Spacewar::UpdateEntities() {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
		// Update each entity based on their object types
		switch ((*iter)->getObjectType()) {
		case OBJECT_TYPE_PLAYER: {
									 //					Key Inputs
									 //=================================================
									 if (input->isKeyDown(VK_UP)) {
										 (*iter)->setVelocity(
											 (cos((*iter)->getRadians()) * playerAccelerationRate + (*iter)->getVelocity().x),
											 (sin((*iter)->getRadians()) * playerAccelerationRate + (*iter)->getVelocity().y)
											 );
									 }

									 if (input->isKeyDown(VK_DOWN)) {
										 (*iter)->setVelocity(
											 (cos((*iter)->getRadians()) * -playerAccelerationRate + (*iter)->getVelocity().x),
											 (sin((*iter)->getRadians()) * -playerAccelerationRate + (*iter)->getVelocity().y)
											 );
									 }

									 if (input->isKeyDown(VK_LEFT)) {
										 if (!(*iter)->hasEffect(EFFECT_INVERTED))
											 (*iter)->setRadians((*iter)->getRadians() - deltaTime * playerTurnMultiplier);
										 else
											 (*iter)->setRadians((*iter)->getRadians() + deltaTime * playerTurnMultiplier);
									 }

									 if (input->isKeyDown(VK_RIGHT)) {
										 if (!(*iter)->hasEffect(EFFECT_INVERTED))
											 (*iter)->setRadians((*iter)->getRadians() + deltaTime * playerTurnMultiplier);
										 else
											 (*iter)->setRadians((*iter)->getRadians() - deltaTime * playerTurnMultiplier);
									 }

									 // brakes
									 if (input->isKeyDown(0x5A)) {
										 (*iter)->setVelocity(
											 (*iter)->getVelocity().x - (*iter)->getVelocity().x * playerDeccelerationRate * 3,
											 (*iter)->getVelocity().y - (*iter)->getVelocity().y * playerDeccelerationRate * 3);
									 }

									 // natural decceleration
									 (*iter)->setVelocity(
										 (*iter)->getVelocity().x - (*iter)->getVelocity().x * playerDeccelerationRate,
										 (*iter)->getVelocity().y - (*iter)->getVelocity().y * playerDeccelerationRate
										 );

									 if (playerDefaultTexture)			// Prevents the default texture from animating
										 player->setCurrentFrame(player->getStartFrame());

									 //					Player Effects
									 //=================================================
									 // iterate through the various effect that the players have
									 // player effects should be applied here. enviroment effects can be recorded here though it is not recommended
									 for (std::map<EffectType, float>::iterator iter_ = player->getEffectTimers()->begin(); iter_ != player->getEffectTimers()->end(); iter_++) {
										 if (iter_->second > 0.0f) {
											 iter_->second -= deltaTime;
											 switch (iter_->first) {
											 case EFFECT_ENLARGED:
											 {
																	 if (player->hasEffect(EFFECT_ENLARGED)) {
																		 player->setScale(shipNS::SCALING * 2);
																	 }
																	 else {
																		 player->setScale(shipNS::SCALING);
																	 }
											 }
											 case EFFECT_STUN: {
																   if ((*iter)->hasEffect(EFFECT_STUN))
																	   (*iter)->setVelocity(0, 0);
											 } break;
											 case EFFECT_INVINCIBLE: {
																		 if ((*iter)->hasEffect(EFFECT_INVINCIBLE) && (*iter)->getCurrentFrame() == (*iter)->getStartFrame())
																		 {
																			 playerDefaultTexture = false;
																			 (*iter)->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, P_INVIN_COLS, &p_invinTextures);
																			 (*iter)->setFrames(P_INVIN_START_FRAME, P_INVIN_END_FRAME);
																			 (*iter)->setCurrentFrame(P_INVIN_START_FRAME);
																			 (*iter)->setFrameDelay(P_INVIN_ANIMATION_DELAY);
																			 (*iter)->setLoop(P_INVIN_LOOP);
																			 (*iter)->setScale(P_INVIN_SCALE);
																			 (*iter)->setRect();
																		 }
																		 else if (!(*iter)->hasEffect(EFFECT_INVINCIBLE) && playerDefaultTexture != true)
																		 {
																			 playerDefaultTexture = true;
																			 (*iter)->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
																			 (*iter)->setCurrentFrame(shipNS::player_START_FRAME);
																		 }
											 } break;
											 case EFFECT_SLOW: {
																   if ((*iter)->hasEffect(EFFECT_SLOW)) {
																	   (*iter)->setVelocity((*iter)->getVelocity().x / 1.05, (*iter)->getVelocity().y / 1.05);
																   }
											 } break;
											 case EFFECT_INVULNERABLE: {
																		   if ((*iter)->hasEffect(EFFECT_INVULNERABLE) && (*iter)->getCurrentFrame() == (*iter)->getStartFrame())
																		   {
																			   playerDefaultTexture = false;
																			   (*iter)->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, P_INVUL_COLS, &p_invulTextures);
																			   (*iter)->setFrames(P_INVUL_START_FRAME, P_INVUL_END_FRAME);
																			   (*iter)->setCurrentFrame(P_INVUL_START_FRAME);
																			   (*iter)->setFrameDelay(P_INVUL_ANIMATION_DELAY);
																			   (*iter)->setLoop(P_INVUL_LOOP);
																			   (*iter)->setScale(P_INVUL_SCALE);
																			   (*iter)->setRect();
																		   }
																		   else if (!(*iter)->hasEffect(EFFECT_INVULNERABLE) && playerDefaultTexture != true)
																		   {
																			   playerDefaultTexture = true;
																			   (*iter)->initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &shipTextures);
																			   (*iter)->setCurrentFrame(shipNS::player_START_FRAME);
																		   }
											 } break;
											 }
										 }
									 }
									 if (player->hasEffect(EFFECT_STUN))
										 player->setVelocity(0, 0);
		} break;
		case OBJECT_TYPE_TRIANGLE: {
									   double dx, dy;			// For tracking the player

									   dx = player->getX() - (*iter)->getX();
									   dy = player->getY() - (*iter)->getY();

									   // 1, 4 quad
									   if (dx > 0)
										   (*iter)->setRadians(atan(dy / dx));
									   // 2, 3 quad
									   else if (dx < 0)
										   (*iter)->setRadians(PI + atan(dy / dx));

									   (*iter)->setVelocity(
										   cos((*iter)->getRadians()) * 50,
										   sin((*iter)->getRadians()) * 50
										   );

									   // Freeze triangle if player has frozen effect
									   if (player->hasEffect(EFFECT_FROZEN)) {
										   (*iter)->setVelocity(0, 0);
									   }
		} break;
		case OBJECT_TYPE_MISSILE: {		// Empty
		} break;
		case OBJECT_TYPE_BLACKHOLE: {
										calculateF(*iter, player);			// calculates force between player and blackhole
		} break;
		}
		(*iter)->update(deltaTime);
	}
}

void Spacewar::KillEntities() {
	std::vector<Entity*>::iterator iter = entities.begin();
	while (iter != entities.end()) {
		if ((*iter)->getHealth() <= 0) {
			// whatever happends on death of the entity goes here
			// death only happens when health drops to 0
			// will be kept simple to prevent modifying the iterator while deleting an item
			switch ((*iter)->getObjectType()) {
			case OBJECT_TYPE_BLACKHOLE: {
											(*iter)->setActive(false);
											(*iter)->setVisible(false);
											iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_BOSS: {
									   (*iter)->setActive(false);
									   (*iter)->setVisible(false);
									   iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_CIRCLE: {
										 (*iter)->setActive(false);
										 (*iter)->setVisible(false);
										 iter = entities.erase(iter);
										 playerScore += genScore(++combo);
			} break;
			case OBJECT_TYPE_PICKUP: {
										 (*iter)->setActive(false);
										 (*iter)->setVisible(false);
										 iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_PLAYER: {
										 (*iter)->setActive(false);
										 (*iter)->setVisible(false);
										 iter = entities.erase(iter);

										 PlaySound(PLAYER_DEAD_SOUND, NULL, SND_ASYNC);

										 this->setGameState(GAME_STATE_GAMEOVER);
			} break;
			case OBJECT_TYPE_SQUARES: {
										  (*iter)->setActive(false);
										  (*iter)->setVisible(false);
										  iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_TRIANGLE: {
										   (*iter)->setActive(false);
										   (*iter)->setVisible(false);
										   iter = entities.erase(iter);
										   playerScore += genScore(++combo);
			} break;
			case OBJECT_TYPE_EXPLOSION: {
											(*iter)->setActive(false);
											(*iter)->setVisible(false);
											iter = entities.erase(iter);
			} break;
			case OBJECT_TYPE_FREEZE: {
										 (*iter)->setActive(false);
										 (*iter)->setVisible(false);
										 iter = entities.erase(iter);
			} break;
			}
		}
		else iter++;
	}

	// remove missiles that are not targeting anything
	std::vector<Missile*>::iterator iter_ = missiles.begin();
	while (iter_ != missiles.end()) {
		if (!(*iter_)->getActive()) {
			iter_ = missiles.erase(iter_);
		}
		else iter_++;
	}
}

// draw all entites
void Spacewar::DrawEntities() {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
		(*iter)->draw();
	}
}

// simple function to calculate what will be the score applied
int Spacewar::genScore(int combo) {
	return 2 + int(pow(combo, 1.5));
}

// a place where collision calculations takes place
void Spacewar::collisions() {
	// temp vector created to store entries that will be appended to entities after the iteration
	std::vector<Entity*> tempVector;
	switch (this->getGameState()) {
	case GAME_STATE_MENU: {
	} break;
	case GAME_STATE_GAME: {
							  VECTOR2 collisionVector;

							  for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
								  Entity* entity = *iter;

								  // run the following code when player collides with the following entity
								  if (player->collidesWith(*entity, collisionVector)) {
									  switch (entity->getObjectType())
									  {
									  case OBJECT_TYPE_BLACKHOLE: {
																	  // Plays sound and kills player if blackhole is touched when player is not invulnerable or invincible
																	  if (!player->hasEffect(EFFECT_INVULNERABLE) || !player->hasEffect((EFFECT_INVINCIBLE))){
																		  player->damage(WEAPON_BLACKHOLE);
																		  PlaySound(PLAYER_DAMAGE_SOUND, NULL, SND_ASYNC);
																		  player->getEffectTimers()->at(EFFECT_INVULNERABLE) = 2.4f;
																		  combo = 0;
																	  }
									  }	break;

									  case OBJECT_TYPE_CIRCLE: {
																   Circle* circle = (Circle*)(*iter);

																   // Damages circle if player is invincible
																   if (player->hasEffect(EFFECT_INVINCIBLE)) {
																	   circle->damage(WEAPON_PLAYER);
																   }

																   // Plays sound and damages player if circle is touched when player is not invulnerable or invincible
																   else if (!player->hasEffect(EFFECT_INVULNERABLE) && !player->hasEffect(EFFECT_INVINCIBLE)) {
																	   PlaySound(PLAYER_DAMAGE_SOUND, NULL, SND_ASYNC);

																	   player->damage(WEAPON_CIRCLE);
																	   player->getEffectTimers()->at(EFFECT_INVULNERABLE) = 2.4f;
																	   combo = 0;
																   }
									  }	break;

									  case OBJECT_TYPE_TRIANGLE: {
																	 Triangle* tri = (Triangle*)(*iter);

																	 // Damages triangle if player is invincible
																	 if (player->hasEffect(EFFECT_INVINCIBLE)) {
																		 tri->damage(WEAPON_PLAYER);
																	 }

																	 // Plays sound and damages player if triangle is touched when player is not invulnerable or invincible
																	 else if (!player->hasEffect(EFFECT_INVULNERABLE) && !player->hasEffect(EFFECT_INVINCIBLE)) {
																		 PlaySound(PLAYER_DAMAGE_SOUND, NULL, SND_ASYNC);

																		 player->damage(WEAPON_TRIANGLE);
																		 player->getEffectTimers()->at(EFFECT_INVULNERABLE) = 2.4f;
																		 combo = 0;
																	 }
									  }	break;
									  case OBJECT_TYPE_PICKUP: {
																   //				  Pickup Effects
																   //=================================================
																   // run the following code when player touches a pickup

																   // pickup cooldown
																   if (!player->hasEffect(EFFECT_CANNOT_PICKUP)) {
																	   Pickup* pickup_ = (Pickup*)entity;

																	   switch (pickup_->getPickupType()) {
																	   case PICKUP_DESTRUCTOR_EXPLOSION: {
																											 Explosion* explosion = new Explosion();
																											 explosion->initialize(this, explosion->getWidth(), explosion->getHeight(), explosionNS::TEXTURE_COLS, &explosionTexture);
																											 explosion->setX(pickup_->getX() + pickup_->getWidth() * pickup_->getScale() / 2 - (explosion->getWidth() / 2 * explosion->getScale()));
																											 explosion->setY(pickup_->getY() + pickup_->getHeight() * pickup_->getScale() / 2 - (explosion->getHeight() / 2 * explosion->getScale()));
																											 explosion->setFrameDelay(explosionNS::ANIMATION_DELAY);
																											 explosion->setCollisionRadius(explosionNS::WIDTH / 2.0f);
																											 tempVector.push_back(explosion);

																											 // play sound async to the game to avoid 'lag'
																											 PlaySound(PICKUP_EXPLODE_SOUND, NULL, SND_ASYNC);

																											 pickup_->respawnPickup();
																	   } break;
																	   case PICKUP_DESTRUCTOR_FREEZE: {
																										  PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);
																										  Freeze* freeze = new Freeze();
																										  freeze->initialize(this, freezeNS::WIDTH, freezeNS::HEIGHT, freezeNS::TEXTURE_COLS, &freezeTexture);
																										  freeze->setFrames(freezeNS::START_FRAME, freezeNS::END_FRAME);
																										  freeze->setCurrentFrame(freezeNS::START_FRAME);
																										  freeze->setX(GAME_WIDTH / 2 - freezeNS::WIDTH / 2 * freezeNS::SCALING);
																										  freeze->setY(GAME_HEIGHT / 2 - freezeNS::WIDTH / 2 * freezeNS::SCALING);
																										  freeze->setFrameDelay(freezeNS::ANIMATION_DELAY);
																										  freeze->setRect();
																										  tempVector.push_back(freeze);
																										  pickup_->respawnPickup();
																										  player->getEffectTimers()->at(EFFECT_FROZEN) = 5.0f;
																	   } break;
																	   case PICKUP_DESTRUCTOR_INVINCIBILITY: {
																												 PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																												 pickup_->respawnPickup();
																												 player->getEffectTimers()->at(EFFECT_INVINCIBLE) = 10.0f;
																	   } break;
																	   case PICKUP_DESTRUCTOR_MISSLES: {
																										   // get the enemies to target first
																										   // discarded after this iteration
																										   std::vector<Entity*> tempVect;
																										   for (std::vector<Entity*>::iterator iter_ = entities.begin(); iter_ != entities.end(); iter_++) {
																											   if (
																												   ((*iter_)->getObjectType() == OBJECT_TYPE_CIRCLE ||
																												   (*iter_)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
																												   (*iter_)->getObjectType() == OBJECT_TYPE_BOSS) &&
																												   (*iter_)->getActive() == true
																												   ) {
																												   tempVect.push_back(*iter_);
																											   }
																										   }

																										   // spawn 5 - 10 missiles
																										   for (int i = 0; i <= minMaxRand(5, 10) && i < tempVect.size(); i++) {
																											   Missile* m = new Missile();
																											   m->initialize(this, 128, 32, 1, &missileTexture);
																											   m->setX(player->getX() + m->getWidth() / 2);
																											   m->setY(player->getY() + m->getHeight() / 2);
																											   m->setTarget(tempVect[i]);
																											   missiles.push_back(m);
																										   }

																										   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																										   pickup_->respawnPickup();
																	   } break;
																	   case PICKUP_HEALTH: {
																							   // no need to reset heart type since there will always be one in a game
																							   //PlaySound(PLAYER_PICKUP_HEART_SOUND, NULL, SND_ASYNC);

																							   // create a new heart pickup
																							   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);
																							   healthPickup = new Pickup();
																							   healthPickup->initialize(this, PickupNS::WIDTH, PickupNS::HEIGHT, PickupNS::TEXTURE_COLS, &heartTexture);
																							   healthPickup->setPickUpType(PICKUP_HEART);
																							   healthPickup->setCurrentFrame(0);
																							   healthPickup->setX(minMaxRand(healthPickup->getWidth(), GAME_WIDTH - 2 * healthPickup->getWidth()));
																							   healthPickup->setY(minMaxRand(healthPickup->getHeight(), GAME_HEIGHT - 2 * healthPickup->getHeight()));

																							   addEntity(healthPickup);

																							   pickup_->calculateObstructorDestructorType();
																							   pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																							   pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));

																							   /*player->setHealth(player->getHealth() + 1);
																							   if (player->getHealth() > 10)
																								   player->setHealth(10);
																							   playerScore += genScore(++combo);
<<<<<<< HEAD
																							   pickup_->setX(minMaxRand(pickup_->getWidth(), GAME_WIDTH - 2 * pickup_->getWidth()));
																							   pickup_->setY(minMaxRand(pickup_->getHeight(), GAME_HEIGHT - 2 * pickup_->getHeight()));*/
																	   } break;
																	   case PICKUP_HEART:{
																		   PlaySound(PLAYER_PICKUP_HEART_SOUND, NULL, SND_ASYNC);

																		   // increase player health by 1
																		   player->setHealth(player->getHealth() + 1);
																		   if (player->getHealth() > 10)
																			   player->setHealth(10);
																		   //playerScore += genScore(++combo);
																		   pickup_->setHealth(0);
																		   pickup_->respawnPickup();

																	   } break;
																	   case PICKUP_OBSTRUCTOR_BLACKHOLE: {
																											 // blackhole is a environmental effect.
																											 // blackhole that is stored in the entities vector will have effect on the gravity. will be killed when the timer reaches 0
																											 Blackhole* blackhole = new Blackhole();
																											 blackhole->initialize(this, blackholeNS::WIDTH, blackholeNS::HEIGHT, blackholeNS::TEXTURE_COLS, &blackHoleTexture);
																											 blackhole->setX(minMaxRand(blackhole->getWidth(), GAME_WIDTH - 2 * blackhole->getWidth()));
																											 blackhole->setY(minMaxRand(blackhole->getWidth(), GAME_WIDTH - 2 * blackhole->getWidth()));

																											 PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																											 pickup_->respawnPickup();

																											 addEntity(blackhole);
																	   } break;
																	   case PICKUP_OBSTRUCTOR_ENLARGE_PLAYER: {
																												  PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																												  pickup_->respawnPickup();
																												  player->getEffectTimers()->at(EFFECT_ENLARGED) = 10.0f;
																	   } break;
																	   case PICKUP_OBSTRUCTOR_INVERT_CONTROLS: {
																												   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																												   pickup_->respawnPickup();
																												   player->getEffectTimers()->at(EFFECT_INVERTED) = 10.0f;
																	   } break;
																	   case PICKUP_OBSTRUCTOR_SLOW_PLAYER: {
																											   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																											   pickup_->respawnPickup();
																											   player->getEffectTimers()->at(EFFECT_SLOW) = 10.0f;
																	   } break;
																	   case PICKUP_OBSTRUCTOR_STUN_PLAYER: {
																											   PlaySound(PLAYER_PICKUP_SOUND, NULL, SND_ASYNC);

																											   pickup_->respawnPickup();
																											   player->getEffectTimers()->at(EFFECT_STUN) = 10.0f;
																	   } break;
																	   }

																	   // pickup cooldown value set here
																	   player->getEffectTimers()->at(EFFECT_CANNOT_PICKUP) = 0.5f;
																   }
									  } break;
									  }
								  }
							  }

							  // adds all entities stored in the temporary vector data structure
							  for (std::vector<Entity*>::iterator iter = tempVector.begin(); iter != tempVector.end(); iter++) {
								  addEntity(*iter);
							  }
	} break;
	case GAME_STATE_SETTING: {
	} break;
	}
}

double Spacewar::calculateF(Entity* e1, Entity* e2) {
	// based on the formula GM1M2/R^2
	//================================

	double deltaX = e1->getX() + e1->getWidth() / 2 * e1->getScale() - e2->getX() - e2->getWidth() * e2->getScale() / 2;
	double deltaY = e1->getY() + e1->getHeight() / 2 * e1->getScale() - e2->getY() - e2->getHeight() * e2->getScale() / 2;
	double distance = sqrt(pow(deltaX, 2) + pow(deltaY, 2));

	// force
	double force = G * e2->getMass() * e1->getMass() / pow(distance, 2);

	// deltaV
	double A1 = force / e1->getMass();
	double A2 = force / e2->getMass();

	e1->setVelocity(
		e1->getVelocity().x + deltaX * A1,
		e1->getVelocity().y + deltaY * A1
		);

	e2->setVelocity(
		e2->getVelocity().x + deltaX * A2,
		e2->getVelocity().y + deltaY * A2
		);

	return force;
}

void Spacewar::ParseScore(std::string fileName) {
	std::string delimiter = ";";
	std::ifstream infile(fileName);
	for (std::string line; std::getline(infile, line);) {
		std::string	name;
		int			score;

		std::string s;

		std::istringstream str_(line);
		while (std::getline(str_, s, ';')) {
			name = s.c_str();
			std::getline(str_, s, ';');
			score = std::stoi(s);
			this->highscores.insert(std::pair<int, std::string>(score, name));
		}
	}
}

void PrintEffect(Entity* entity, Font* effectFont) {
	//	  Displays each pickup status on the GUI
	// ============================================

	float dy = 10;
	for (std::map<EffectType, float>::iterator iter = entity->getEffectTimers()->begin(); iter != entity->getEffectTimers()->end(); iter++) {
		if (entity->hasEffect((*iter).first)) {
			ss.str("");
			switch ((*iter).first) {
			case EFFECT_INVERTED: {
									  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Inverted";
									  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
									  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_SLOW: {
								  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Slowed";
								  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
								  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_STUN: {
								  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Stunned";
								  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
								  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_INVULNERABLE: {
										  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Invulnerable";
										  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
										  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_FROZEN: {
									ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Frozen";
									effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
									dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_ENLARGED: {
									  ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Enlarged";
									  effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
									  dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			case EFFECT_INVINCIBLE: {
										ss << std::fixed << std::setprecision(1) << (float)((*iter).second) << " Invincible";
										effectFont->Print(GAME_WIDTH - 20 - effectFont->getTotalWidth(ss.str()), dy, ss.str());
										dy += effectFont->getHeight() * effectFont->getScale();
			} break;
			}
		}
	}
}

bool isWaveOver(std::vector<Entity*> entities) {
	for (std::vector<Entity*>::iterator iter = entities.begin(); iter != entities.end(); iter++) {
		if ((
			(*iter)->getObjectType() == OBJECT_TYPE_BOSS ||
			(*iter)->getObjectType() == OBJECT_TYPE_TRIANGLE ||
			(*iter)->getObjectType() == OBJECT_TYPE_CIRCLE ||
			(*iter)->getObjectType() == OBJECT_TYPE_BLACKHOLE) &&
			(*iter)->getActive() == true
			)
			return false;
	}
	return true;
}

bool isTargeted(std::vector<Missile*> missiles, Entity* entity) {
	for (std::vector<Missile*>::iterator iter = missiles.begin(); iter != missiles.end(); iter++) {
		if ((*iter)->getTarget() == entity)
			return true;
	}
	return false;
}

int minMaxRand(int min, int max) {
	return rand() % (max - min + 1) + min;
}
