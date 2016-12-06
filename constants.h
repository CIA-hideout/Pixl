// Programming 2D Games
// Copyright (c) 2011 by:
// Charles Kelly
// Chapter 6 constants.h v1.0

#ifndef _CONSTANTS_H            // Prevent multiple definitions if this
#define _CONSTANTS_H            // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#define PI	3.14159265
#define G	6.67428e-11f

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

//-----------------------------------------------
// Useful macros
//-----------------------------------------------
// Safely delete pointer referenced item
#define SAFE_DELETE(ptr)       { if (ptr) { delete (ptr); (ptr)=NULL; } }
// Safely release pointer referenced item
#define SAFE_RELEASE(ptr)      { if(ptr) { (ptr)->Release(); (ptr)=NULL; } }
// Safely delete pointer referenced array
#define SAFE_DELETE_ARRAY(ptr) { if(ptr) { delete [](ptr); (ptr)=NULL; } }
// Safely call onLostDevice
#define SAFE_ON_LOST_DEVICE(ptr)    { if(ptr) { ptr->onLostDevice(); } }
// Safely call onResetDevice
#define SAFE_ON_RESET_DEVICE(ptr)   { if(ptr) { ptr->onResetDevice(); } }
#define TRANSCOLOR  SETCOLOR_ARGB(0,0,0,0)  // transparent color (magenta)

//-----------------------------------------------
//                  Constants
//-----------------------------------------------

// window
const char CLASS_NAME[] = "Pixl";
const char GAME_TITLE[] = "Pixl";
const bool FULLSCREEN = false;              // windowed or fullscreen
const UINT GAME_WIDTH = 1200;               // width of game in pixels
const UINT GAME_HEIGHT = 1000;               // height of game in pixels

// game
const float FRAME_RATE = 200.0f;                // the target frame rate (frames/sec)
const float MIN_FRAME_RATE = 10.0f;             // the minimum frame rate
const float MIN_FRAME_TIME = 1.0f / FRAME_RATE;   // minimum desired time for 1 frame
const float MAX_FRAME_TIME = 1.0f / MIN_FRAME_RATE; // maximum time used in calculations
const float MASS_PLANET = 1.0e14f;
const float MASS_SHIP = 5.0f;

// player
const int P_DEATH_WIDTH					= 256;						// player death width
const int P_DEATH_HEIGHT				= 256;						// player death height
const int P_DEATH_COLS					= 8;						// player death columns
const int P_DEATH_START_FRAME			= 0;						// player death start frame number
const int P_DEATH_END_FRAME				= 25;						// player death end frame number
const float P_DEATH_SCALE				= 0.5f;						// player starting scale
const float P_DEATH_ANIMATION_DELAY		= 0.42f;					// player death animation time between frames
const bool P_DEATH_LOOP					= false;					// player death animation loop
const int P_INVUL_COLS					= 4;						// player invul columns
const int P_INVUL_START_FRAME			= 0;						// player invul start frame number
const int P_INVUL_END_FRAME				= 8;						// player invul end frame number
const float P_INVUL_SCALE				= 0.5f;						// player invul starting scale
const float P_INVUL_ANIMATION_DELAY		= 0.1f;						// player invul animation time between frames
const bool P_INVUL_LOOP					= true;						// player invul animation loop	
const int P_INVIN_COLS					= 2;						// player invin columns
const int P_INVIN_START_FRAME			= 0;						// player invin start frame number
const int P_INVIN_END_FRAME				= 4;						// player invin end frame number
const float P_INVIN_SCALE				= 0.5f;						// player invin starting scale
const float P_INVIN_ANIMATION_DELAY		= 0.1f;						// player invin animation time between frames
const bool P_INVIN_LOOP					= true;						// Player invin animation loop


//=================================
//		RESOURCE FILE PATHS
//=================================

// Player
const char PLAYER_TEXTURE[]					= "resources\\images\\player.png";
const char PLAYER_DEATH_TEXTURE[]			= "resources\\images\\player_death_animation.png";
const char PLAYER_INVUL_TEXTURE[]			= "resources\\images\\player_invul_animation_spritesheet.png";
const char PLAYER_INVIN_TEXTURE[]			= "resources\\images\\player_invin_animation_spritesheet.png";

// Enemy
const char TRIANGLE_TEXTURE[]				= "resources\\images\\triangle.png";
const char CIRCLE_TEXTURE[]					= "resources\\images\\circle.png";
const char BLACKHOLE_TEXTURE[]				= "resources\\images\\blackhole.png";

// Pickups
const char DESTRUCTOR_OBSTRUCTOR_TEXTURE[]	= "resources\\images\\destructor_obstructor.png";
const char MISSILE_TEXTURE[]				= "resources\\images\\missile.png";
const char EXPLOSION_TEXTURE[]				= "resources\\images\\explosion.png";

// GUI
const char FONT_TEXTURE[]					= "resources\\images\\font.png";
const char FONT_TEXTURE_INFO[]				= "resources\\data\\font.dat";
const char HEART_TEXTURE[] 					= "resources\\images\\sprite_hearts.png";

// Sound 
const char PLAYER_PICKUP_SOUND[]			= "resources\\sounds\\pickup_2.wav";		// player pick up a pickup
const char PLAYER_PICKUP_HEART_SOUND[]		= "resources\\sounds\\pickup_heart.wav";	// player pickup a heart
const char PLAYER_DAMAGE_SOUND[]			= "resources\\sounds\\damage_player.wav";	// player gets damaged
const char PLAYER_SELECT_SOUND[]			= "resources\\sounds\\push_button.wav";		// player pushes space or esc
const char PICKUP_EXPLODE_SOUND[]			= "resources\\sounds\\explosion.wav";		// explosion pickup, well explodes

const char HIGHSCORE_FILE[]					= "resources\\data\\highscore.dat";


// key mappings
// In this game simple constants are used for key mappings. If variables were used
// it would be possible to save and restore key mappings from a data file.
const UCHAR ESC_KEY = VK_ESCAPE;       // escape key
const UCHAR ALT_KEY = VK_MENU;         // Alt key
const UCHAR ENTER_KEY = VK_RETURN;       // Enter key
const UCHAR SHIP_ROTATE_LEFT = VK_LEFT;
const UCHAR SHIP_ROTATE_RIGHT = VK_RIGHT;
const UCHAR SHIP_ADD_V = VK_UP;
const UCHAR SHIP_SUBTRACT_V = VK_DOWN;
const UCHAR SPACEBAR = 0x20;

// weapon types
enum WEAPON {
	WEAPON_TRIANGLE,
	WEAPON_CIRCLE,
	WEAPON_BLACKHOLE,
	WEAPON_PLAYER,
	WEAPON_BOSS,
	WEAPON_MISSILE,
	WEAPON_EXPLOSION
};
#endif
