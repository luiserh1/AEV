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
#define NUM_SPRITES 15
#define NUM_TEXTURES 7

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
static C2D_Image textures[NUM_TEXTURES]; // All the sprites
static int totalImages = NUM_SPRITES + NUM_SPRITES;

void initFrog()
{
	player.state = IDLE;
	player.x = 0;
	player.y = jumpSize * numRows / 2;
} 

void initSprites()
{	
	for (int i = 0; i < NUM_SPRITES; i++)
	{
		C2D_Sprite* sprite = &sprites[i];
		C2D_SpriteFromSheet(sprite, spriteSheet, i);
		C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
	}
	for (int i = NUM_SPRITES; i < totalImages; i++)
	{
		textures[i - NUM_SPRITES] = C2D_SpriteSheetGetImage(spriteSheet, i);
	}
}

C2D_Image getImage(int id) { return textures[id - NUM_SPRITES]; }

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

void drawGrid()
{	
	u32 clrGrey = GREY; 
	for (int i = 0; i < numRows; i++)
	{
		C2D_DrawRectSolid(0, SCREENS_HEIGHT - i * jumpSize, 0, TOP_SCREEN_WIDTH, 1, clrGrey);
	}
	for (int i = 0; i < numColumns; i++)
	{
		C2D_DrawRectSolid(TOP_SCREEN_WIDTH - i * jumpSize, 0, 0, 1, SCREENS_HEIGHT, clrGrey);
	}
}

void drawTerrainImages() 
{
	C2D_Image texture;
	switch (gameSize) 
	{
		case SMALL:
			texture = getImage(sprites_Sand_Small_idx);
			break;
		case MEDIUM:
			texture = getImage(sprites_Sand_Medium_idx);
			break;
		case BIG:
			texture = getImage(sprites_Sand_Big_idx);
			break;
	}
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numColumns; j++)
		{
			int x = i * jumpSize, y = j * jumpSize;
			C2D_DrawImageAt(texture, x, y, 0, NULL, 1.0f, 1.0f);
			printf("(%d, %d)\n", x, y);
		}
	}
}

void drawUI()
{

}

int main(int argc, char* argv[])
{
	// Inits
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);	// Initialize citro3D (required for citro2D drawing)
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);	// Initialize citro2D
	C2D_Prepare();						// Prepares the GPU for rendering 2D content

	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT); // The action of the game will occur here
	C3D_RenderTarget* bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT); 

	int gridEnabled = false;
	setGameSize(SMALL);

	//consoleInit(GFX_BOTTOM, NULL);
	// Main loop
	while (aptMainLoop())
	{
		gfxSwapBuffers(); // I do not not if this is useful
		hidScanInput();
		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kDown & KEY_A) gridEnabled = !gridEnabled; // A turs the grid ON/OFF
		if (kDown & KEY_LEFT) // D-PAD LEFT changes the size of the grid
			changeGameSize();


		u32 backgroundTop = WHITE;
		u32 backgroundBot = BLUE;
		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(top, backgroundTop);
			C2D_SceneBegin(top);

			if (gridEnabled) drawGrid();
			drawTerrain();


			C2D_TargetClear(bot, backgroundBot);
			C3D_FrameDrawOn(bot);
			C2D_SceneBegin(bot);
			C2D_DrawImageAt(getImage(sprites_Bottom_Screen_Menu_idx), 0, 0, 0, NULL, 1.0f, 1.0f);
		C3D_FrameEnd(0);
		C2D_Flush();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}