#include <citro2d.h>
#include <citro3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <time.h>

#include <sprites.h>

// Screen size definitions
#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH  320
#define SCREENS_HEIGHT 240

// To define the game size
#define SMALL 1
#define MEDIUM 2
#define BIG 3
#define GRID_SQUARE_SIDE_SMALL 5


// Frog's states
#define IDLE 0
#define JUMPING 1

// Create colors
#define WHITE C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF)
#define GREY C2D_Color32(0xD3, 0xD3, 0xD3, 0xA0)
#define BLACK C2D_Color32(0x00, 0x00, 0x00, 0xFF)
#define RED C2D_Color32(0xFF, 0x00, 0x00, 0xFF)
#define GREEN C2D_Color32(0x00, 0xFF, 0x00, 0xFF)
#define BLUE C2D_Color32(0x00, 0x00, 0xFF, 0xFF)

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

void initFrog()
{
	player.state = IDLE;
	player.x = 0;
	player.y = jumpSize * numRows / 2;
} 

void setGameSize(int newSize)
{
	gameSize = newSize;
	int sideSize = GRID_SQUARE_SIDE_SMALL;
	for (int i = 1; i < gameSize; i++) sideSize *= 2;
	if(gameSize > BIG) gameSize = SMALL; // The size selection is cyclic
	numRows = SCREENS_HEIGHT / sideSize;
	numColumns = TOP_SCREEN_WIDTH / sideSize; 
	jumpSize = SCREENS_HEIGHT / numRows;
}

void changeGameSize() { setGameSize(gameSize + 1); }

void drawGrid()
{	
	u32 clrGrey = GREY; 
	for (int i = 0; i < numRows; i++)
	{
		C2D_DrawRectSolid(TOP_SCREEN_WIDTH - i * TOP_SCREEN_WIDTH / numRows, 0, 0, 1, SCREENS_HEIGHT, clrGrey);
		C2D_DrawRectSolid(0, SCREENS_HEIGHT - i * SCREENS_HEIGHT / numRows, 0, TOP_SCREEN_WIDTH, 1, clrGrey);
	}
	for (int i = 0; i < numRows; i++)
	{
		C2D_DrawRectSolid(TOP_SCREEN_WIDTH - i * TOP_SCREEN_WIDTH / numRows, 0, 0, 1, SCREENS_HEIGHT, clrGrey);
		C2D_DrawRectSolid(0, SCREENS_HEIGHT - i * SCREENS_HEIGHT / numRows, 0, TOP_SCREEN_WIDTH, 1, clrGrey);
	}
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
	// Main loop
	while (aptMainLoop())
	{
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

			C2D_TargetClear(bot, backgroundBot);
			C3D_FrameDrawOn(bot);
			C2D_SceneBegin(bot);
		C3D_FrameEnd(0);
		//C2D_Flush();
	}

	gfxExit();
	return 0;
}