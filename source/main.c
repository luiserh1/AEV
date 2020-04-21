#include <citro2d.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <time.h>

#include <sprites.h>
#include "colors.h"

// Screen size definitions
#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH  320
#define SCREENS_HEIGHT 240

// The number of sprites has to be changed manually
#define NUM_SPRITES 52

// To define the game size
#define SMALL 1
#define MEDIUM 2
#define BIG 3
#define GRID_SQUARE_SIDE_SMALL 20

// Frog's states
#define IDLE 0
#define JUMPING 1


typedef struct 
{
	int state;		// State of the frog's state machine
	int x, y;		// Coords to represent the position in the grid (sprite center [0, 0])
	C2D_Sprite spr;	// Sprite to draw the frog
} Frog;	// To represent the player

static int gameSize = MEDIUM;
static int numRows, numColumns;
static int jumpSize;
static Frog player;
static C2D_SpriteSheet spriteSheet; // The tex3ds spritesheet
static C2D_Sprite sprites[NUM_SPRITES]; // All the sprites
static C2D_Sprite terrainSprites[TOP_SCREEN_WIDTH / GRID_SQUARE_SIDE_SMALL][SCREENS_HEIGHT / GRID_SQUARE_SIDE_SMALL];
static bool buttonPressed;
static float capacity = 1.0f;

void initSpritesAndFrog()
{	
	// All the sprites get this default configuration; Many of them will change it during run
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		C2D_Sprite* sprite = &sprites[i];
		C2D_SpriteFromSheet(sprite, spriteSheet, i);
		C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
	}

	// Capacitable screen menu Layout
	C2D_SpriteSetCenter(&sprites[sprites_Bottom_Screen_Menu_idx], 0.0f, 0.0f);
	C2D_SpriteSetPos(&sprites[sprites_Bottom_Screen_Menu_idx], 0, 0);
	C2D_SpriteSetDepth(&sprites[sprites_Bottom_Screen_Menu_idx], 0.0f); 

	// Pressable and pressed button go on the same opsition obv.
	C2D_SpriteSetCenter(&sprites[sprites_Button_Not_Pressed_idx], 0.0f, 0.0f);
	C2D_SpriteSetPos(&sprites[sprites_Button_Not_Pressed_idx], 181, 177); // The coords have been obtained using GIMP
	C2D_SpriteSetDepth(&sprites[sprites_Button_Not_Pressed_idx], 1.0f); 
	C2D_SpriteSetCenter(&sprites[sprites_Button_Pressed_idx], 0.0f, 0.0f);
	C2D_SpriteSetPos(&sprites[sprites_Button_Pressed_idx], 181, 177);
	C2D_SpriteSetDepth(&sprites[sprites_Button_Pressed_idx], 1.0f); 

	// The barrel
	C2D_SpriteSetCenter(&sprites[sprites_Barrel_Empty_idx], 0.5f, 1.0f); 	// The pivot is set in this position to reduce the level of
	C2D_SpriteSetPos(&sprites[sprites_Barrel_Empty_idx], 64, 201); 			// the content easily just scaling
	C2D_SpriteSetDepth(&sprites[sprites_Barrel_Empty_idx], 1.0f); 			
	// The content
	C2D_SpriteSetCenter(&sprites[sprites_Barrel_Content_idx], 0.5f, 1.0f); 	
	C2D_SpriteSetPos(&sprites[sprites_Barrel_Content_idx], 64, 198); 		// The barrel is a bit higher than the content because it's 3 pixels bigger
	C2D_SpriteSetDepth(&sprites[sprites_Barrel_Content_idx], 0.5f); 


	// About the player starting position
	player.state = IDLE;
	player.x = 0;
	player.y = jumpSize * numRows / 2;
	player.spr = sprites[sprites_Frog_Standing_Small_idx];
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

void setGameSize(int newSize)
{
	gameSize = newSize;
	if(gameSize > BIG) gameSize = SMALL; // The size selection is cyclic
	int sideSize = GRID_SQUARE_SIDE_SMALL;
	for (int i = BIG; i > gameSize; i--) sideSize *= 2;
	numRows = SCREENS_HEIGHT / sideSize;
	numColumns = TOP_SCREEN_WIDTH / sideSize; 
	jumpSize = sideSize;
}

void changeGameSize() { setGameSize(gameSize + 1); }

void drawUI()
{
	C2D_DrawSprite(&sprites[sprites_Bottom_Screen_Menu_idx]);
	if (buttonPressed) C2D_DrawSprite(&sprites[sprites_Button_Pressed_idx]);
	else C2D_DrawSprite(&sprites[sprites_Button_Not_Pressed_idx]);

	C2D_SpriteSetScale(&sprites[sprites_Barrel_Content_idx], 1.0f, 1.0f * capacity);
	C2D_DrawSprite(&sprites[sprites_Barrel_Content_idx]); // El contenido debe renderizarse antes
	C2D_DrawSprite(&sprites[sprites_Barrel_Empty_idx]);

}

void drawGrid()
{	
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

void drawPlayer()
{
	C2D_SpriteSetPos(&player.spr, player.x, player.y);
	C2D_DrawSprite(&player.spr);
}

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

	int gridEnabled = false;
	setGameSize(SMALL);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);
	initSpritesAndFrog();
	//consoleInit(GFX_BOTTOM, NULL);
	// Main loop
	while (aptMainLoop())
	{
		setTerrainSprites();

		gfxSwapBuffers(); // I do not not if this is useful

		buttonPressed = false;
		if (capacity > 0) capacity -= 0.001f;
		hidScanInput();

		// Checking the input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kDown & KEY_A) gridEnabled = !gridEnabled; // A turs the grid ON/OFF
		if (kDown & KEY_LEFT) // D-PAD LEFT changes the size of the grid
			changeGameSize();

		u32 kHeld = hidKeysHeld();
		if(kHeld & KEY_B) buttonPressed = true;


		u32 backgroundTop = WHITE;
		u32 backgroundBot = BLUE;
		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, backgroundTop);
			C2D_SceneBegin(top);

			for (int i = 0; i < numColumns; i++) 
				for (int j = 0; j < numRows; j++)
					C2D_DrawSprite(&terrainSprites[i][j]);
			if (gridEnabled) drawGrid();
			drawPlayer();
			
			C2D_TargetClear(bot, backgroundBot);
			C3D_FrameDrawOn(bot);
			C2D_SceneBegin(bot);
			drawUI();

		C3D_FrameEnd(0);
		C2D_Flush();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}