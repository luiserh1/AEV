#define DEBBUG_CONTROLS true

#include <citro2d.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <time.h>

#include <menu_sprites.h>
#include <frog.h>
#include <items.h>
#include <menu_images.h>
#include <obstacles.h>
#include <particles.h>
#include <terrain.h>

#include "colors.h"
#include "sin1.h" // author stfwi from https://www.atwillys.de/content/cc/sine-lookup-for-embedded-in-c/?lang=en
#include "intArithmetic.h"
#include "animation.h"

// Screen size definitions
#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH  320
#define SCREENS_HEIGHT 240

// Q15 number representing a full turn (360 degrees / 2*PI radians)
#define FULL_TURN 32766

// Time per animation frame
#define TIME_PER_FRAME_MILIS 50;

// To define the game size
#define GRID_SQUARE_SIDE_SMALL 20
// Game sizes
enum GAME_SIZE { SMALL, MEDIUM, BIG };
static char gameSizeNames[3][10] = 
{
	{'S','m','a','l','l',' ',' ',' ',' ',' '},
	{'M','e','d','i','u','m',' ',' ',' ',' '},
	{'B','i','g',' ',' ',' ',' ',' ',' ',' '}
};
// Frog's states
enum STATE { SPAWNING, IDLE, JUMPING, DYING };
static char stateNames[4][10] =					
{
	{'S','p','a','w','n','i','n','g',' ',' '},
	{'I','d','l','e',' ',' ',' ',' ',' ',' '},
	{'J','u','m','p','i','n','g',' ',' ',' '},
	{'D','y','i','n','g',' ',' ',' ',' ',' '}
};
// Frog's orientations
enum ORIENTATION { UP, DOWN, RIGHT, LEFT, IRRELEVANT };
static char orientationNames[4][10] = 
{
	{'U','p',' ',' ',' ',' ',' ',' ',' ',' '},
	{'D','o','w','n',' ',' ',' ',' ',' ',' '},
	{'R','i','g','h','t',' ',' ',' ',' ',' '},
	{'L','e','f','t',' ',' ',' ',' ',' ',' '}
};
// Pickeable items
enum ITEMS { HAT_CHINGON, NONE };
static char itemNames[2][10] = 
{
	{'H','a','t','C','h','i','n','g','o','n'},
	{'N','o','n','e',' ',' ',' ',' ',' ',' '}
};
// Obstacles
enum OBSTACLES { CACTUS, EMPTY };
static char obstaclesNames[2][10] = 
{
	{'C','a','c','t','u','s',' ',' ',' ',' '},
	{'E','m','p','t','y',' ',' ',' ',' ',' '}
};
// Terrain
enum TERRAIN { WATER, SAND };
static char terrainNames[2][10] = 
{
	{'W','a','t','e','r',' ',' ',' ',' ',' '},
	{'S','a','n','d',' ',' ',' ',' ',' ',' '}
};


// To represent the player
typedef struct 
{
	enum STATE state;					// State of the frog's state machine
	enum ORIENTATION orientation;		// Orientation wich the frog is facing
	int x, y;							// Coords to represent the position in the grid
	SpriteAnimation *anim;				// Sprite to draw the frog
} Frog;	

void drawUIMenu();
void initGame();
void setTerrain();
void setGameFromSize(enum GAME_SIZE newSize);
void decrementGameSize();
void incrementGameSize();
void drawGrid();
void resetPlayerProperties();
void setOrientation(enum ORIENTATION orientation);
void jump(enum ORIENTATION orientation);
void move(enum ORIENTATION orientation);
void incrementFliesCount();
void incrementLivesCount();
void decrementLivesCount();
void nextPhase();
void processGameplayControls(u32 kDown, u32 kHeld, touchPosition touch);
void processDebugControls(u32 kDown);
void drawItem();
void useItem();
void showBottomScreen(C3D_RenderTarget* bot);
void showTopScreen(C3D_RenderTarget* top);
void showConsole();

static int gameSize = MEDIUM;	// Each level may have a different number of tiles
static int gridEnabled = false; // This is a debug option to see all of the tiles
static bool buttonPressed;	// True if the UI button is pressed this cycle

static int numRows, numColumns;	// This variables will contain the values to know how many are
static int jumpSize;			// This also represent the size of the tile's side
static Frog player;				// The player
	
// The tex3ds spritesheets
static C2D_SpriteSheet frogSpriteSheet;			// Sprites for the frog's representation
static C2D_SpriteSheet itemsSpriteSheet;		// Sprites for the pickable items' representation
static C2D_SpriteSheet menuImagesSpriteSheet;	// Images for the menu's representation
static C2D_SpriteSheet menuSpritesSpriteSheet;	// Sprites for the menu's representation
static C2D_SpriteSheet obstaclesSpriteSheet;	// Sprites for the obstacles' representation
static C2D_SpriteSheet particlesSpriteSheet;	// Sprites for the particles' representation
static C2D_SpriteSheet terrainSpriteSheet; 		// Images for the level terrain's representation

// Top screen animations for...
// ... the frog's states
static SpriteAnimationFrame idleFrogFrames[3][1];
static SpriteAnimation frogIdleAnimations[3];
static SpriteAnimationFrame jumpingFrogFrames[3][30];
static SpriteAnimation frogJumpingAnimations[3];
// ... the wereable hat
static SpriteAnimationFrame wereableHatFrames[3][1];
static SpriteAnimation wereableHatAnimations[3];
// ... the terrain
static ImageAnimationFrame terrainFrames[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL][1];
static ImageAnimation terrainAnimations[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL];
// ... the obstacles
static ImageAnimationFrame obstaclesFrames[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL][1];
static ImageAnimation obstaclesAnimations[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL];

// Bottom Screen animations for...
// ... the layout (images)
static ImageAnimationFrame layoutFrames[1];
static ImageAnimation layoutAnimation;
// ... the button (images)
static ImageAnimationFrame buttonFrames[1];
static ImageAnimation buttonAnimation;
static ImageAnimationFrame buttonPressedFrames[1];
static ImageAnimation buttonPressedAnimation;
// ... the life/death icons (sprites)
static SpriteAnimationFrame lifeIconsFrames[3][1];
static SpriteAnimation lifeIconsAnimations[3];
static SpriteAnimationFrame deathIconsFrames[3][1];
static SpriteAnimation deathIconsAnimations[3];
// ... the flies icons (sprites)
static SpriteAnimationFrame flyIconsFrames[3][2];
static SpriteAnimation flyIconsAnimations[3];
// ... the water barrel (sprites)
static SpriteAnimationFrame barrelFrontFrames[1];
static SpriteAnimation barrelFrontAnimation;
static SpriteAnimationFrame barrelContentFrames[100];
static SpriteAnimation barrelContentAnimation;
static SpriteAnimationFrame barrelBackFrames[1];
static SpriteAnimation barrelBackAnimation;
// ... the phase numbers (sprites)
static SpriteAnimationFrame phaseNumbersSprites[9][20];
static SpriteAnimation phaseNumbersAnimations[9];
// ... the items (sprites)
static SpriteAnimationFrame itemsSprites[1][20];
static SpriteAnimation itemsAnimations[1];

// Game state variables
static int fliesCount;				// Getting three flies equals to 1UP
static int livesCount;				// To a max. of 3 lives (0 life counts)
static int currentPhase;			// Number corresponding to the current phase
static bool chingonMode;			// Special model where the capacity does not drop down
static enum ITEMS currentItem;		// Current item in the box

// Terrain elements distribution
static enum OBSTACLES obstacles[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL];
static enum TERRAIN terrain[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL];
static enum ITEMS items[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL];

static bool consoleMode;			// In console mode it's possible to visualize more data
									// Once you active it there is not going back (debug only)

//////////////////////
// SET UP FUNCTIONS //
//////////////////////

void initGame()
{
	//============//
	// ANIMATIONS //
	//============//

	// Setting up the frog animations
	for (int i = 0; i < 3; i++)
	{
		idleFrogFrames[i][0].duration = 1;
		C2D_Sprite *spriteIdle = &idleFrogFrames[i][0].spr;
		C2D_SpriteFromSheet(spriteIdle, frogSpriteSheet, frog_Frog_Standing_Big_idx - i); // IMPORTANT they are one after each other on the spritesheet
		C2D_SpriteSetCenter(spriteIdle, 0.5f, 0.5f);
		C2D_SpriteSetDepth(spriteIdle, 0.75f);
		//C2D_SpriteSetPos(spriteIdle, 100 + 90 * i, 75); We specify the pos when animating the moves
		for (int j = 0; j < 30; j++)
		{
			jumpingFrogFrames[i][j].duration = 5;
			C2D_Sprite *spriteJumpingJ = &jumpingFrogFrames[i][j].spr;
			if (j > 3 && j < 27) C2D_SpriteFromSheet(spriteJumpingJ, frogSpriteSheet, frog_Frog_Jumping_Big_idx - i); 	// Since we do not have many sprites the first
			else C2D_SpriteFromSheet(spriteJumpingJ, frogSpriteSheet, frog_Frog_Standing_Big_idx - i); 			// and last sprite of the jumping animation are
			C2D_SpriteSetCenter(spriteJumpingJ, 0.5f, 0.5f);													// just the frog standing sprites.
			C2D_SpriteSetDepth(spriteJumpingJ, 0.75f);															// TODO add more and different sprites
			//C2D_SpriteSetPos(spriteJumpingJ,100 + 90 * i, 165); This positions were for testing porpuses											
		}

		setUpSpriteAnimation(&frogIdleAnimations[i], &idleFrogFrames[i][0], 1, true);
		setUpSpriteAnimation(&frogJumpingAnimations[i], &jumpingFrogFrames[i][0], 30, false);
	}

	// Setting up the wereable hat animations
	for (int i = 0; i < 3; i++)
	{
		wereableHatFrames[i][0].duration = 1;
		C2D_Sprite *hatSprite = &wereableHatFrames[i][0].spr;
		C2D_SpriteFromSheet(hatSprite, frogSpriteSheet, frog_Hat_Chingon_Big_Above_idx - i);
		C2D_SpriteSetCenter(hatSprite, 0.5f, 0.5f);													// just the frog standing sprites.
		C2D_SpriteSetDepth(hatSprite, 0.85f);			
		C2D_SpriteSetPos(hatSprite, 100 + 90 * i, 165);
		setUpSpriteAnimation(&wereableHatAnimations[i], &wereableHatFrames[i][0], 1, true);	
	}
	
	// Setting up the layout's animations
	layoutFrames[0].duration = 1;
	layoutFrames[0].coords.x = 0;
	layoutFrames[0].coords.y = 0;
	layoutFrames[0].depth = -1.0f;
	C2D_Image *img = &layoutFrames[0].img;
	*img = C2D_SpriteSheetGetImage(menuImagesSpriteSheet, menu_images_Bottom_Screen_Menu_idx);
	setUpImageAnimation(&layoutAnimation, &layoutFrames[0], 1);

	// Setting up the life/death icons' animations
	for (int i = 0; i < 3; i++)
	{
		lifeIconsFrames[i][0].duration = 1;
		deathIconsFrames[i][0].duration = 1;
		C2D_Sprite *spriteLife = &lifeIconsFrames[i][0].spr;
		C2D_Sprite *spriteDeath = &deathIconsFrames[i][0].spr;
		C2D_SpriteFromSheet(spriteLife, menuSpritesSpriteSheet, menu_sprites_Frog_Life_Icon_idx);
		C2D_SpriteFromSheet(spriteDeath, menuSpritesSpriteSheet, menu_sprites_Frog_Death_Icon_idx);
		C2D_SpriteSetCenter(spriteLife, 0.5f, 0.5f);
		C2D_SpriteSetCenter(spriteDeath, 0.5f, 0.5f);
		C2D_SpriteSetDepth(spriteLife, 0.0f);		
		C2D_SpriteSetDepth(spriteDeath, 0.0f);			
		C2D_SpriteSetPos(spriteLife, 270 - i * 40, 40);	
		C2D_SpriteSetPos(spriteDeath, 270 - i * 40, 40);
		setUpSpriteAnimation(&lifeIconsAnimations[i], &lifeIconsFrames[i][0], 1, true);
		setUpSpriteAnimation(&deathIconsAnimations[i], &deathIconsFrames[i][0], 1, true);
	}

	// Setting up the button's animations
	buttonFrames[0].duration = 1;
	buttonFrames[0].coords.x = 181;
	buttonFrames[0].coords.y = 177;
	layoutFrames[0].depth = 0.0f;
	img = &buttonFrames[0].img;
	*img = C2D_SpriteSheetGetImage(menuImagesSpriteSheet, menu_images_Button_Not_Pressed_idx);
	setUpImageAnimation(&buttonAnimation, &buttonFrames[0], 1);
	buttonPressedFrames[0].duration = 9999;
	buttonPressedFrames[0].coords.x = 181;
	buttonPressedFrames[0].coords.y = 177;
	layoutFrames[0].depth = 0.0f;
	img = &buttonPressedFrames[0].img;
	*img = C2D_SpriteSheetGetImage(menuImagesSpriteSheet, menu_images_Button_Pressed_idx);
	setUpImageAnimation(&buttonPressedAnimation, &buttonPressedFrames[0], 1);

	// Setting up the barrels's animations
	barrelBackFrames[0].duration = 1;
	C2D_Sprite *sprite = &barrelBackFrames[0].spr;
	C2D_SpriteFromSheet(sprite, menuSpritesSpriteSheet, menu_sprites_Barrel_Inside_idx);
	C2D_SpriteSetCenter(sprite, 0.5f, 1.0f);
	C2D_SpriteSetDepth(sprite, 0.25f);			
	C2D_SpriteSetPos(sprite, 64, 201);	
	setUpSpriteAnimation(&barrelBackAnimation, &barrelBackFrames[0], 1, true);
	barrelFrontFrames[0].duration = 9999;
	sprite = &barrelFrontFrames[0].spr;
	C2D_SpriteFromSheet(sprite, menuSpritesSpriteSheet, menu_sprites_Barrel_Empty_idx);
	C2D_SpriteSetCenter(sprite, 0.5f, 1.0f);
	C2D_SpriteSetDepth(sprite, 0.75f);			
	C2D_SpriteSetPos(sprite, 64, 201);	
	setUpSpriteAnimation(&barrelFrontAnimation, &barrelFrontFrames[0], 1, true);
	for (int i = 0; i < 100; i++) 
	{
		barrelContentFrames[i].duration = 6;
		C2D_Sprite *spriteContent = &barrelContentFrames[i].spr;
		C2D_SpriteFromSheet(spriteContent, menuSpritesSpriteSheet, menu_sprites_Barrel_Content_idx);
		C2D_SpriteSetCenter(spriteContent, 0.5f, 1.0f);
		C2D_SpriteSetDepth(spriteContent, 0.65f);			
		C2D_SpriteSetPos(spriteContent, 64, 198);	
		C2D_SpriteSetScale(spriteContent, 1.0f, 1.0f - 0.01f * i);
	}
	setUpSpriteAnimation(&barrelContentAnimation, &barrelContentFrames[0], 100, false);

	// Setting up the flies icons' animations
	for (int i = 0; i < 3; i++)
	{
		flyIconsFrames[i][0].duration = 10;
		flyIconsFrames[i][1].duration = 20;
		C2D_Sprite *spriteWingsUp = &flyIconsFrames[i][0].spr;
		C2D_Sprite *spriteWingsDown = &flyIconsFrames[i][1].spr;
		C2D_SpriteFromSheet(spriteWingsUp, menuSpritesSpriteSheet, menu_sprites_Fly_1_Big_idx);
		C2D_SpriteFromSheet(spriteWingsDown, menuSpritesSpriteSheet, menu_sprites_Fly_2_Big_idx);
		C2D_SpriteSetCenter(spriteWingsUp, 0.5f, 0.5f);
		C2D_SpriteSetCenter(spriteWingsDown, 0.5f, 0.5f);
		C2D_SpriteSetDepth(spriteWingsUp, 0.0f);		
		C2D_SpriteSetDepth(spriteWingsDown, 0.0f);			
		C2D_SpriteSetPos(spriteWingsUp, 125, 101 + 38 * i);	
		C2D_SpriteSetPos(spriteWingsDown,  125, 101 + 38 * i);
		setUpSpriteAnimation(&flyIconsAnimations[i], &flyIconsFrames[i][0], 2, true);
	}

	// Setting up the phase numbers animation
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			phaseNumbersSprites[i][j].duration = 5;
			C2D_Sprite *sprite = &phaseNumbersSprites[i][j].spr;
			if (j < 10)
			{
				C2D_SpriteFromSheet(sprite, menuSpritesSpriteSheet, menu_sprites_Number_0_idx + i);
				C2D_SpriteSetScale(sprite, 1.0f - 0.1f * j, 1.0f - 0.1f * j);

			}
			else
			{
				C2D_SpriteFromSheet(sprite, menuSpritesSpriteSheet, menu_sprites_Number_1_idx + i);
				C2D_SpriteSetScale(sprite, 0.1f + 0.1f * (j - 10), 0.1f + 0.1f * (j -10));
			}
			C2D_SpriteSetPos(sprite, 143, 40);
			C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
			C2D_SpriteSetDepth(sprite, 0.0f);
		}
		setUpSpriteAnimation(&phaseNumbersAnimations[i], &phaseNumbersSprites[i][0], 20, false);
	}

	// Setting up the items animations
	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			itemsSprites[i][j].duration = 5;
			C2D_Sprite *sprite = &itemsSprites[i][j].spr;
			C2D_SpriteFromSheet(sprite, itemsSpriteSheet, items_Hat_Chingon_Big_idx);
			C2D_SpriteSetPos(sprite, 230, 125);
			C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
			C2D_SpriteSetDepth(sprite, 0.0f);
			//C2D_SpriteSetRotationDegrees(sprite, q15ToFloat(sin1(FULL_TURN / 20 * j)) * 20.0f);
			C2D_SpriteSetRotationDegrees(sprite, q15ToFloat(sin1(floatToQ15(1.0f / 20 * j))) * 20.0f);
		}
		setUpSpriteAnimation(&itemsAnimations[i], &itemsSprites[i][0], 20, true);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//==================//
	// TERRAIN ELEMENTS //
	//==================//

	// Base terrain
	/*terrain =
	{

	};*/

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//===============//
	// GAMEPLAY INIT //
	//===============//

	// This will set the terrain and the player configuration
	setGameFromSize(SMALL);
}

void setGameFromSize(enum GAME_SIZE newSize)
{
	buttonPressed = false;
	currentPhase = 0;
	gameSize = newSize;
	if(gameSize > BIG) gameSize = SMALL; // The size selection is cyclic
	else if(gameSize < SMALL) gameSize = BIG;
	int sideSize = GRID_SQUARE_SIDE_SMALL;
	for (int i = BIG; i > gameSize; i--) sideSize *= 2;
	numRows = SCREENS_HEIGHT / sideSize;
	numColumns = TOP_SCREEN_WIDTH / sideSize; 
	jumpSize = sideSize;

	// The terrain and the player properties change depending on the siz of the game
	setTerrain();
	resetPlayerProperties();
}

void decrementGameSize() { setGameFromSize(gameSize - 1); }
void incrementGameSize() { setGameFromSize(gameSize + 1); }

void resetPlayerProperties()
{
	chingonMode = false;
	currentItem = NONE;
	livesCount = 1;
	fliesCount = 1;

	int tileOffset = jumpSize / 2;
	int rowPos = numRows / 2;
	int colPos = 0;
	for (int i = 0; i < frogIdleAnimations[gameSize].totalFrames; i++)
	{
		C2D_SpriteSetPos(&frogIdleAnimations[gameSize].frames[i].spr, colPos * jumpSize + tileOffset, rowPos * jumpSize + tileOffset);
		setOrientation(RIGHT);
	}
	for (int i = 0; i < wereableHatAnimations[gameSize].totalFrames; i++)
		C2D_SpriteSetPos(&wereableHatFrames[gameSize][i].spr, colPos * jumpSize + tileOffset, rowPos * jumpSize + tileOffset);
	player.anim = &frogIdleAnimations[gameSize];
	resetSpriteAnimation(player.anim);
	resetSpriteAnimation(&barrelContentAnimation);
	player.state = IDLE;
	player.x = 0;
	player.y = numRows / 2;
	setOrientation(RIGHT);
}

void setTerrain()
{
	int sandSpriteSize;
	if (gameSize == SMALL) sandSpriteSize = terrain_Sand_Big_idx;
	else if (gameSize == MEDIUM) sandSpriteSize = terrain_Sand_Medium_idx;
	else sandSpriteSize = terrain_Sand_Big_idx;
	for (int i = 0; i < numColumns; i++) 
	{
		for (int j = 0; j < numRows; j++)
		{
			terrainFrames[i][j][0].coords.x = i * jumpSize;
			terrainFrames[i][j][0].coords.y = j * jumpSize;
			terrainFrames[i][j][0].depth = 0.0f;
			C2D_Image *img = &terrainFrames[i][j][0].img;
			*img = C2D_SpriteSheetGetImage(terrainSpriteSheet, sandSpriteSize);
			setUpImageAnimation(&terrainAnimations[i][j], &terrainFrames[i][j][0], 1);
		}
	}
}

////////////////////////
// GAMPLAY PROCESSING //
////////////////////////

void setOrientation(enum ORIENTATION orientation)
{
	if (orientation == IRRELEVANT) return;
	player.orientation = orientation;
	float degrees = 0;
	switch (orientation)
	{
		case RIGHT: degrees = 90; break;
		case DOWN: degrees = 180; break;
		case LEFT: degrees = 270; break;
		default: case UP: break;
	}
	for (int i = 0; i < player.anim->totalFrames; i++)
		C2D_SpriteSetRotationDegrees(&player.anim->frames[i].spr, degrees);
}

void jump(enum ORIENTATION orientation)
{
	player.state = JUMPING;
	int deltaX = (orientation == RIGHT) * jumpSize - (orientation == LEFT) * jumpSize;
	int deltaY = (orientation == DOWN) * jumpSize - (orientation == UP) * jumpSize; // The y axis are upside down

	Coords oldCoords = getCoordsFromSpriteAnimation(player.anim);
	int totalFrames = frogJumpingAnimations[gameSize].totalFrames;
	for (int i = 0; i < totalFrames; i++)
	{
		C2D_SpriteSetPos(&frogJumpingAnimations[gameSize].frames[i].spr,
			oldCoords.x + deltaX * q15ToFloat(sin1(floatToQ15(1.0f / 4 * i / totalFrames))),
			oldCoords.y + deltaY * q15ToFloat(sin1(floatToQ15(1.0f / 4 * i / totalFrames))));
	}
	player.anim = &frogJumpingAnimations[gameSize];
}

void idle()
{
	player.state = IDLE;
	Coords oldCoords = getCoordsFromSpriteAnimation(player.anim);
	int totalFrames = frogIdleAnimations[gameSize].totalFrames;
	for (int i = 0; i < totalFrames; i++)
	{
		C2D_SpriteSetPos(&frogIdleAnimations[gameSize].frames[i].spr, oldCoords.x, oldCoords.y);
	}
	player.anim = &frogIdleAnimations[gameSize];
}

void move(enum ORIENTATION orientation)
{
	bool movement = true;
	if (player.state == DYING || player.state == SPAWNING) return;
	if (orientation == IRRELEVANT)
		movement = false;
	else
	{
		if (orientation != player.orientation) movement = false;
		else jump(orientation);
	}
	if (!movement) idle();
	resetSpriteAnimation(player.anim);
	setOrientation(orientation);

}

void processDebugControls(u32 kDown)
{
	if (kDown & KEY_A) gridEnabled = !gridEnabled; // A turns the grid ON/OFF
	if (kDown & KEY_R) // R and L buttons change the size of the grid
		incrementGameSize();
	if (kDown & KEY_L)
		decrementGameSize();
	if (kDown & KEY_ZL)
		incrementFliesCount();
	if (kDown & KEY_ZR)
		nextPhase();
	if (kDown & KEY_X) currentItem = HAT_CHINGON;
	if (kDown & KEY_Y && !consoleMode) showConsole();
}

void processGameplay(u32 kDown, u32 kHeld, touchPosition touch)
{
	bool animationEnd = hasEndedSpriteAnimation(player.anim);

	if (animationEnd)
	{
		if (kDown & KEY_LEFT)
			move(LEFT);
		else if (kDown & KEY_RIGHT)
			move(RIGHT);
		else if (kDown & KEY_DOWN)
			move(DOWN);
		else if (kDown & KEY_UP)
			move(UP);
		else move(IRRELEVANT);
	}
	
	// You can use the item pressing the button on the bottom screen or hitting B
	if(kHeld & KEY_B || (touch.px > 181 && touch.px < 278 && touch.py > 177 &&  touch.py < 207)) 
	{
		if (!buttonPressed) buttonPressed = true;
	}
	else if (buttonPressed)
	{
		buttonPressed = false;
		useItem();
	}
	
}

void useItem()
{
	switch (currentItem)
	{
		case HAT_CHINGON:
			chingonMode = true;
			break;
		default: break;
	}
	currentItem = NONE;
}

void incrementFliesCount()
{
	if (++fliesCount > 3)
	{ 
		fliesCount = 0;
		incrementLivesCount();
	}
}

void incrementLivesCount()
{
	if (livesCount < 3) { livesCount++; }
}

void decrementLivesCount()
{
	if (livesCount > -1) { livesCount--; }
}

void nextPhase()
{
	if (++currentPhase > 9) { currentPhase = 0; }
}

///////////////////////
// DRAWING FUNCTIONS //
///////////////////////

void drawGrid() // Using rectangles to draw lines might not be the most efficient way to do
{				// but since it's GPU work I guess it's better than going pixel by pixel
	u32 gridClr = RED; 
	for (int i = 0; i < numRows; i++)
	{
		C2D_DrawRectSolid(0, SCREENS_HEIGHT - i * jumpSize, 1.0f, TOP_SCREEN_WIDTH, 1, gridClr);
	}
	for (int i = 0; i < numColumns; i++)
	{
		C2D_DrawRectSolid(TOP_SCREEN_WIDTH - i * jumpSize, 1.0f, 0, 1, SCREENS_HEIGHT, gridClr);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void drawItem()
{
	if (currentItem == NONE) return;
	renderSpriteAnimation(&itemsAnimations[currentItem], 1);
}

void showBottomScreen(C3D_RenderTarget* bot)
{
	u32 backgroundBot = BLUE;
	C2D_TargetClear(bot, backgroundBot);
	//C3D_FrameDrawOn(bot); I do not know what is this for, but it seems like I do not need it
	C2D_SceneBegin(bot);
	// The layout
	renderImageAnimation(&layoutAnimation, 1);

	// The life and death icons
	for (int i = 0; i < 3; i++)
	{
		if (i < livesCount) renderSpriteAnimation(&lifeIconsAnimations[i], 1);
		else renderSpriteAnimation(&deathIconsAnimations[i], 1);
	}
	// The button
	if (buttonPressed) renderImageAnimation(&buttonPressedAnimation, 1);
	else renderImageAnimation(&buttonAnimation, 1);

	// The barrel and its content
	renderSpriteAnimation(&barrelBackAnimation, 1);
	renderSpriteAnimation(&barrelContentAnimation, 1);
	renderSpriteAnimation(&barrelFrontAnimation, 1);


	for (int i = 0; i < fliesCount; i++)
		renderSpriteAnimation(&flyIconsAnimations[i], 1); // TODO: Not animation for the 0

	// The phase number
	renderSpriteAnimation(&phaseNumbersAnimations[currentPhase], 1);

	// The item
	drawItem();
}

void showTopScreen(C3D_RenderTarget* top)
{
	u32 backgroundTop = BLUE;
	C2D_TargetClear(top, backgroundTop);
	C2D_SceneBegin(top);

	// The terrain
	for (int i = 0; i < numColumns; i++) 
	{
		for (int j = 0; j < numRows; j++)
		{
			renderImageAnimation(&terrainAnimations[i][j], 1);
		}
	}

	// The player
	renderSpriteAnimation(player.anim, 1);
	if (chingonMode) 
	{
		// The hat moves with the frog
		Coords newCoords = getCoordsFromSpriteAnimation(player.anim);
		for (int i = 0; i < wereableHatAnimations[gameSize].totalFrames; i++)
		{	// The trembling effect is not intended, but it's cool
			C2D_SpriteSetPos(&wereableHatFrames[gameSize][i].spr, newCoords.x, newCoords.y);
		}
		renderSpriteAnimation(&wereableHatAnimations[gameSize], 1);
	}

	// The (debug) grid
	if (gridEnabled) drawGrid();

}

void showConsole()
{
	consoleInit(GFX_BOTTOM, NULL);
	consoleMode = true;
}

///////////////////
// MAIN FUNCTION //
///////////////////
int main(int argc, char* argv[])
{
	// Inits
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);	// Initialize citro3D (required for citro2D drawing)
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);	// Initialize citro2D
	C2D_Prepare();						// Prepares the GPU for rendering 2D content

	C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT); // The action of the game will occur here
	C3D_RenderTarget *bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT); 

	// Load graphics
	frogSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/frog.t3x");
	itemsSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/items.t3x");
	menuImagesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/menu_images.t3x");
	menuSpritesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/menu_sprites.t3x");
	obstaclesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/obstacles.t3x");
	particlesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/particles.t3x");
	terrainSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/terrain.t3x");
	if (!frogSpriteSheet || !itemsSpriteSheet || !menuImagesSpriteSheet || !menuSpritesSpriteSheet
		|| !obstaclesSpriteSheet || !particlesSpriteSheet || !terrainSpriteSheet) svcBreak(USERBREAK_PANIC);
	
	initGame();

	consoleMode = false;
	// Main loop
	while (aptMainLoop())
	{	
		if (consoleMode)
		{
			consoleClear();
			// Debug area
			printf("Game Size: %.10s\n", gameSizeNames[gameSize]);
			printf("State: %.10s\n", stateNames[player.state]);
			printf("Orientation: %.10s\n", orientationNames[player.orientation]);
			printf("Current Frame(%d/%d)\nFrame Process(%d/%d)\n",
				player.anim->currentAnimationFrame + 1, player.anim->totalFrames,
				player.anim->frames[player.anim->currentAnimationFrame].currentFrame + 1,
				player.anim->frames[player.anim->currentAnimationFrame].duration);
			Coords oldCoords = getCoordsFromSpriteAnimation(player.anim);
			printf("Coords(%d, %d)\n", oldCoords.x, oldCoords.y);
			printf("\n========================================\nLives: %d\n", livesCount);
			printf("Flies: %d\n", fliesCount);
			printf("Current Item: %.10s\n", itemNames[currentItem]);
			printf("Chingon Mode: %d\n", chingonMode);
			printf("Capacity: %.2f\n", 
				(barrelContentAnimation.totalFrames - barrelContentAnimation.currentAnimationFrame)
				/ (float)barrelContentAnimation.totalFrames);
		}
		// Checking the input
		hidScanInput();
		u32 kDown = hidKeysDown(); 	// Read thebuttons pressed this cycle
		u32 kHeld = hidKeysHeld();	// Read the held buttons
		touchPosition touch;
		hidTouchRead(&touch); 		// Read the touch screen coordinates
		// Debug Controls
		if (DEBBUG_CONTROLS) processDebugControls(kDown);
		// Gameplay controls
		processGameplay(kDown, kHeld, touch);
		
		gfxSwapBuffers(); // We need this to display things
		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);	
			// The top screen has the all the movement related gameplay sprites and drawings
			showTopScreen(top);
		//C3D_FrameEnd(0);
		C2D_Flush();
		//C3D_FrameBegin(C3D_FRAME_SYNCDRAW);	
			// The bottom screen has the all UI related sprites and drawings
			if (!consoleMode) showBottomScreen(bot);
		C3D_FrameEnd(0);
		C2D_Flush();
		gfxSwapBuffers(); // We need this to display things

		if (kDown & KEY_START)
			break;
	}

	gfxExit();
	return 0;
}