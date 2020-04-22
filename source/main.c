#define DEBBUG_CONTROLS true
#define BOTTOM_SCREEN_CONSOLE true

#include <citro2d.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <time.h>

#include <sprites.h>
#include "colors.h"
#include "sin1.h" // author stfwi from https://www.atwillys.de/content/cc/sine-lookup-for-embedded-in-c/?lang=en
#include "intArithmetic.h"

// Screen size definitions
#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH  320
#define SCREENS_HEIGHT 240

// Q15 number representing a full turn (360 degrees / 2*PI radians)
#define FULL_TURN 32766

// To define the game size
#define SMALL 0
#define MEDIUM 1
#define BIG 2
#define GRID_SQUARE_SIDE_SMALL 20

// Frog's states
enum STATE { IDLE, JUMPING, DYING };
enum ORIENTATION { UP, DOWN, RIGHT, LEFT };
enum ITEMS { HAT_CHINGON, NONE };

// To represent the player
typedef struct 
{
	enum STATE state;			// State of the frog's state machine
	enum ORIENTATION orientation;	// Orientation wich the frog is facing
	int x, y;					// Coords to represent the position in the grid
	C2D_Sprite* spr;				// Sprite to draw the frog
} Frog;	

void drawUIMenu();
void initGame();
void setTerrainSprites();
void setGameFromSize(int newSize);
void decrementGameSize();
void incrementGameSize();
void drawGrid();
void drawPlayer();
void drawJumpingPlayer();
void drawIdlePlayer();
void resetPlayerProperties();
void resetPlayerSpriteProperties();
void setOrientation(enum ORIENTATION orientation);
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

static int gameSize = MEDIUM;	// Each level may have a different number of tiles
static int gridEnabled = false; // This is a debug option to see all of the tiles
static bool buttonPressed;	// True if the UI button is pressed this cycle

static int numRows, numColumns;	// This variables will contain the values to know how many are
static int jumpSize;			// This also represent the size of the tile's side
static Frog player;				// The player

static C2D_SpriteSheet spriteSheet; 	// The tex3ds spritesheet
static C2D_Sprite terrainSprites[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL];
static C2D_Sprite fliesIconSprites[3];
static C2D_Sprite lifeIconsSprites[3];
static C2D_Sprite deathIconsSprites[3];
static C2D_Sprite numbersSprites[10];
static C2D_Sprite frogIdleSprites[3];
static C2D_Sprite frogJumpingSprites[3];
static C2D_Sprite wearableHatSprites[3];
static C2D_Sprite pickableHatSprites[3];
static C2D_Sprite UIItemsSprites[1];
static C2D_Sprite buttonPressedSprite, buttonNotPressedSprite;
static C2D_Sprite barrelEmptySprite, barrelContentSprite;
static C2D_Sprite UILayout;
static C2D_Sprite *currentJumpingSprite, *currentIdleSprite, *currentHatSprite;


static float capacity;		// Represents the water left
static int fliesCount;		// Getting three flies equals to 1UP
static int livesCount;		// Getting three flies equals to 1UP
static int currentPhase;	// Number corresponding to the current phase
static bool chingonMode;
static enum ITEMS currentItem;

//////////////////////
// CONFIG FUNCTIONS //
//////////////////////
void initGame()
{
	// The frog's sprites in all their variants
	for (int i = 0; i < 3; i++)
	{
		C2D_Sprite* spriteIdle = &frogIdleSprites[i];
		C2D_Sprite* spriteJumping = &frogJumpingSprites[i];
		C2D_SpriteFromSheet(spriteIdle, spriteSheet, sprites_Frog_Standing_Big_idx - i); // IMPORTANT they are followed on sprites.t3s
		C2D_SpriteFromSheet(spriteJumping, spriteSheet, sprites_Frog_Jumping_Big_idx - i); // IMPORTANT they are followed on sprites.t3s
		C2D_SpriteSetCenter(spriteIdle, 0.5f, 0.5f);
		C2D_SpriteSetCenter(spriteJumping, 0.5f, 0.5f);
		C2D_SpriteSetDepth(spriteIdle, 1.0f); 
		C2D_SpriteSetDepth(spriteJumping, 1.0f); 
	}

	// The hats YIIIIIIHAAAWWWWLLLLEYYYY!
	for (int i = 0; i < 3; i++)
	{
		C2D_Sprite* spriteWearable = &wearableHatSprites[i];
		C2D_Sprite* spritePickable = &pickableHatSprites[i];
		C2D_SpriteFromSheet(spriteWearable, spriteSheet, sprites_Hat_Chingon_Big_Above_idx - i); // IMPORTANT they are followed on sprites.t3s
		C2D_SpriteFromSheet(spritePickable, spriteSheet, sprites_Hat_Chingon_Big_idx - i); // IMPORTANT they are followed on sprites.t3s
		C2D_SpriteSetCenter(spriteWearable, 0.5f, 0.5f);
		C2D_SpriteSetCenter(spritePickable, 0.5f, 0.5f);
		C2D_SpriteSetDepth(spriteWearable, 1.0f); // DO NOT PUT THE DEPTH VALUE OVER 1.0f!!!!!!
		C2D_SpriteSetDepth(spritePickable, 1.0f); 
	}

	// The items
	C2D_SpriteFromSheet(&UIItemsSprites[HAT_CHINGON], spriteSheet, sprites_Hat_Chingon_Big_idx);
	for (int i = 0; i < 1; i++)
	{
		C2D_Sprite* sprite = &UIItemsSprites[i];
		C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
		C2D_SpriteSetPos(sprite, 230, 125);
		C2D_SpriteSetDepth(sprite, 1.0f);
	}	 

	// Capacitable screen menu Layout
	C2D_SpriteFromSheet(&UILayout, spriteSheet, sprites_Bottom_Screen_Menu_idx);
	C2D_SpriteSetCenter(&UILayout, 0.0f, 0.0f);
	C2D_SpriteSetPos(&UILayout, 0, 0);
	C2D_SpriteSetDepth(&UILayout, 0.0f); 

	// The fly icon sprites
	for (int i = 0; i < 3; i++)
	{
		C2D_Sprite* sprite = &fliesIconSprites[i];
		C2D_SpriteFromSheet(sprite, spriteSheet, sprites_Fly_1_Big_idx);
		C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
		C2D_SpriteSetPos(sprite, 125, 101 + 38 * i); // 38 are the pixeles between the frame's centers
		C2D_SpriteSetDepth(sprite, 1.0f); // The will be displayed over the layout
	}

	// The numbers sprites
	for (int i = 0; i < 10; i++)
	{
		C2D_Sprite* sprite = &numbersSprites[i];
		C2D_SpriteFromSheet(sprite, spriteSheet, sprites_Number_0_idx + i); // IMPORTANT they are followed on sprites.t3s
		C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
		C2D_SpriteSetPos(sprite, 143, 40); // They share the same position because they are not meant to appear at the same time
		C2D_SpriteSetDepth(sprite, 1.0f); // The will be displayed over the layout
	}

	// The life and death icon sprites
	for (int i = 0; i < 6; i++)
	{
		int j = i;
		C2D_Sprite* sprite;
		if (i < 3)
		{
			sprite = &lifeIconsSprites[j];
			C2D_SpriteFromSheet(sprite, spriteSheet, sprites_Frog_Life_Icon_idx); // IMPORTANT they are followed on sprites.t3s
		}
		else
		{
			j -= 3;
			sprite = &deathIconsSprites[j];
			C2D_SpriteFromSheet(sprite, spriteSheet, sprites_Frog_Death_Icon_idx); // IMPORTANT they are followed on sprites.t3s
		}
		C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
		C2D_SpriteSetPos(sprite, 270 - j * 40, 40); // 40 are the pixeles between the Lives holders
		C2D_SpriteSetDepth(sprite, 1.0f); // The will be displayed over the layout
	}

	// Pressable and pressed button go on the same position obv.
	C2D_SpriteFromSheet(&buttonNotPressedSprite, spriteSheet, sprites_Button_Not_Pressed_idx);
	C2D_SpriteSetCenter(&buttonNotPressedSprite, 0.0f, 0.0f);
	C2D_SpriteSetPos(&buttonNotPressedSprite, 181, 177); // The coords have been obtained using GIMP
	C2D_SpriteSetDepth(&buttonNotPressedSprite, 1.0f); 
	C2D_SpriteFromSheet(&buttonPressedSprite, spriteSheet, sprites_Button_Pressed_idx);
	C2D_SpriteSetCenter(&buttonPressedSprite, 0.0f, 0.0f);
	C2D_SpriteSetPos(&buttonPressedSprite, 181, 177);
	C2D_SpriteSetDepth(&buttonPressedSprite, 1.0f); 

	// The barrel
	C2D_SpriteFromSheet(&barrelEmptySprite, spriteSheet, sprites_Barrel_Empty_idx);
	C2D_SpriteSetCenter(&barrelEmptySprite, 0.5f, 1.0f); 	// The pivot is set in this position to reduce the level of
	C2D_SpriteSetPos(&barrelEmptySprite, 64, 201); 			// the content easily just scaling
	C2D_SpriteSetDepth(&barrelEmptySprite, 1.0f); 			// The will be displayed over the layout
	// The content
	C2D_SpriteFromSheet(&barrelContentSprite, spriteSheet, sprites_Barrel_Content_idx);
	C2D_SpriteSetCenter(&barrelContentSprite, 0.5f, 1.0f); 	
	C2D_SpriteSetPos(&barrelContentSprite, 64, 198); 		// The barrel is a bit higher than the content because it's 3 pixels bigger
	C2D_SpriteSetDepth(&barrelContentSprite, 0.5f); 		// The will be displayed over the layout and under the barrel

	// This will set the terrain and the player configuration
	setGameFromSize(SMALL);
}

void setGameFromSize(int newSize)
{
	buttonPressed = false;
	currentPhase = 0;
	gameSize = newSize;
	if(gameSize > BIG) gameSize = SMALL; // The size selection is cyclic
	if(gameSize < SMALL) gameSize = BIG;
	int sideSize = GRID_SQUARE_SIDE_SMALL;
	for (int i = BIG; i > gameSize; i--) sideSize *= 2;
	numRows = SCREENS_HEIGHT / sideSize;
	numColumns = TOP_SCREEN_WIDTH / sideSize; 
	jumpSize = sideSize;

	currentJumpingSprite = &frogJumpingSprites[gameSize];
	currentIdleSprite = &frogIdleSprites[gameSize];
	currentHatSprite = &wearableHatSprites[gameSize];

	// The terrain and the player properties change depending on the siz of the game
	setTerrainSprites();
	resetPlayerProperties();
}

void decrementGameSize() { setGameFromSize(gameSize - 1); }
void incrementGameSize() { setGameFromSize(gameSize + 1); }

void setOrientation(enum ORIENTATION orientation) { player.orientation = orientation; }

void incrementFliesCount()
{
	if (++fliesCount > 3)
	{ 
		fliesCount = 0;
		livesCount++;
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

void resetPlayerProperties()
{
	chingonMode = false;
	currentItem = NONE;
	livesCount = 1;
	capacity = 1.0f;
	fliesCount = 1;
	player.state = IDLE;
	player.x = 0;
	player.y = numRows / 2;
	player.orientation = RIGHT;
	resetPlayerSpriteProperties();
}

void resetPlayerSpriteProperties()
{
	switch (player.orientation)
	{
		case UP: 	C2D_SpriteSetRotationDegrees(player.spr, 0); break;
		case DOWN: 	C2D_SpriteSetRotationDegrees(player.spr, 180); break;
		case LEFT: 	C2D_SpriteSetRotationDegrees(player.spr, 270); break;
		case RIGHT: C2D_SpriteSetRotationDegrees(player.spr, 90); break;
	}
	C2D_SpriteSetCenter(player.spr, 0.5f, 0.5f); 	
}

void setTerrainSprites()
{
	int sandSpriteSize;
	if (gameSize == SMALL) sandSpriteSize = sprites_Sand_Big_idx;
	else if (gameSize == MEDIUM) sandSpriteSize = sprites_Sand_Medium_idx;
	else sandSpriteSize = sprites_Sand_Small_idx;
	for (int i = 0; i < numColumns; i++) 
	{
		for (int j = 0; j < numRows; j++)
		{
			C2D_Sprite* sprite = &terrainSprites[i][j];
			C2D_SpriteFromSheet(sprite, spriteSheet, sandSpriteSize);
			C2D_SpriteSetCenter(sprite, 0.0f, 0.0f);
			C2D_SpriteSetPos(sprite, i * jumpSize, j * jumpSize);
		}
	}
}

///////////////////////
// DRAWING FUNCTIONS //
///////////////////////

void drawGrid() // Using rectangles to draw lines might not be the most efficient way to do
{				// but since it's GPU work I guess it's better than going pixel by pixel
	u32 gridClr = RED; 
	for (int i = 0; i < numRows; i++)
	{
		C2D_DrawRectSolid(0, SCREENS_HEIGHT - i * jumpSize, 0, TOP_SCREEN_WIDTH, 1, gridClr);
	}
	for (int i = 0; i < numColumns; i++)
	{
		C2D_DrawRectSolid(TOP_SCREEN_WIDTH - i * jumpSize, 0, 0, 1, SCREENS_HEIGHT, gridClr);
	}
}

void drawIdlePlayer()
{
	player.spr = currentIdleSprite;
	drawPlayer();
}

void drawJumpingPlayer()
{
	player.spr = currentJumpingSprite;
	drawPlayer();
}

void drawPlayer()
{
	resetPlayerSpriteProperties();
	int tileOffset = jumpSize / 2; // To set the sprite on the center of the tile
	C2D_SpriteSetPos(player.spr, player.x * jumpSize + tileOffset, player.y * jumpSize + tileOffset);
	C2D_DrawSprite(player.spr);
	if (chingonMode)
	{
		C2D_SpriteSetPos(currentHatSprite, player.x * jumpSize + tileOffset, player.y * jumpSize + tileOffset);
		C2D_DrawSprite(currentHatSprite);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void drawUIMenu()
{
	// The layout
	C2D_DrawSprite(&UILayout);
	// The button
	if (buttonPressed) C2D_DrawSprite(&buttonPressedSprite);
	else C2D_DrawSprite(&buttonNotPressedSprite); 

	// The barrel and its content
	C2D_SpriteSetScale(&barrelContentSprite, 1.0f, 1.0f * capacity);
	C2D_DrawSprite(&barrelContentSprite); 
	C2D_DrawSprite(&barrelEmptySprite);

	// The fly icons
	for (int i = 0; i < fliesCount; i++)
		C2D_DrawSprite(&fliesIconSprites[i]);

	// The life and death icons
	for (int i = 0; i < 3; i++) 
	{
		if (i < livesCount) C2D_DrawSprite(&lifeIconsSprites[i]);
		else C2D_DrawSprite(&deathIconsSprites[i]);
	}

	// The phase number
	C2D_DrawSprite(&numbersSprites[currentPhase]);

	drawItem();
}

void drawItem()
{
	if (currentItem == NONE) return;
	C2D_DrawSprite(&UIItemsSprites[currentItem]);
}

//////////////////////
// INPUT PROCESSING //
//////////////////////

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
}

void processGameplayControls(u32 kDown, u32 kHeld, touchPosition touch)
{
	if (kDown & KEY_LEFT)
	{
		setOrientation(LEFT);
	}
	if (kDown & KEY_RIGHT)
	{
		setOrientation(RIGHT);
	}
	if (kDown & KEY_DOWN)
	{
		setOrientation(DOWN);
	}
	if (kDown & KEY_UP)
	{
		setOrientation(UP);
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
			currentItem = NONE;
			break;
		default: break;
	}
}

void showBottomScreen(C3D_RenderTarget* bot)
{
	u32 backgroundBot = BLUE;
	C2D_TargetClear(bot, backgroundBot);
	//C3D_FrameDrawOn(bot); I do not know what is this for, but it seems like I do not need it
	C2D_SceneBegin(bot);
	drawUIMenu();
}

void showTopScreen(C3D_RenderTarget* top)
{
	u32 backgroundTop = WHITE;
	C2D_TargetClear(top, backgroundTop);
	C2D_SceneBegin(top);
	for (int i = 0; i < numColumns; i++) 
		for (int j = 0; j < numRows; j++)
			C2D_DrawSprite(&terrainSprites[i][j]);
	if (gridEnabled) drawGrid();
	drawIdlePlayer();
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

	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT); // The action of the game will occur here
	C3D_RenderTarget* bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT); 

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);
	initGame();
	
	if (BOTTOM_SCREEN_CONSOLE)
	{
		consoleInit(GFX_BOTTOM, NULL);
		printf("Debug console initiallized\n");
	   	printf("Sin1 of 0 %f\n", q15ToFloat(sin1(0)));
		printf("Sin1 of 45 %f\n", q15ToFloat(sin1(FULL_TURN / 8)));
		printf("Sin1 of 90 %f\n", q15ToFloat(sin1(FULL_TURN / 4))); 
		printf("Cos1 of 0 %f\n", q15ToFloat(cos1(0)));
		printf("Cos1 of 45 %f\n", q15ToFloat(cos1(45)));
		printf("Cos1 of 90 %f\n", q15ToFloat(cos1(90)));

		printf("\nFloatToQ of 0 %d\n", floatToQ15(0));
		printf("FloatToQ of 0.125f %d\n", floatToQ15(0.125f));
		printf("FloatToQ of 0.25f %d\n", floatToQ15(0.25f));
	}

	// Main loop
	while (aptMainLoop())
	{
		gfxSwapBuffers(); // We need this to display things
		if (capacity > 0) capacity -= 0.001f;
		
		// Checking the input
		hidScanInput();
		u32 kDown = hidKeysDown(); 	// Read thebuttons pressed this cycle
		u32 kHeld = hidKeysHeld();	// Read the held buttons
		touchPosition touch;
		hidTouchRead(&touch); 		// Read the touch screen coordinates
		// Debug Controls
		if (DEBBUG_CONTROLS) processDebugControls(kDown);
		// Gameplay controls
		processGameplayControls(kDown, kHeld, touch);
		
		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);	
			// The top screen has the all the movement related gameplay sprites and drawings
			showTopScreen(top);
			// The bottom screen has the all UI related sprites and drawings
			if (!BOTTOM_SCREEN_CONSOLE) showBottomScreen(bot);
		C3D_FrameEnd(0);
		C2D_Flush();
		gfxSwapBuffers(); // We need this to display things

		if (kDown & KEY_START)
			break;
	}

	gfxExit();
	return 0;
}