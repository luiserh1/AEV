#include <citro2d.h>
#include <citro3d.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <time.h>

#include <sprites.h>

#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH  320
#define SCREENS_HEIGHT 240

#define SMALL 0
#define MEDIUM 1
#define BIG 2
#define VERY_BIG 3
#define GRID_COLUMNS_VERY_BIG 25
#define GRID_ROWS_VERY_BIG 20
#define GRID_COLUMNS_BIG 20
#define GRID_ROWS_BIG 16
#define GRID_COLUMNS_MEDIUM 15
#define GRID_ROWS_MEDIUM 12
#define GRID_COLUMNS_SMALL 10 
#define GRID_ROWS_SMALL 8

// Create colors
#define WHITE C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF)
#define GREY C2D_Color32(0x88, 0x88, 0x88, 0xA0)
#define BLACK C2D_Color32(0x00, 0x00, 0x00, 0xFF)

static int gameSize = MEDIUM;

void drawGrid()
{	
	int numColumns = 1, numRows = 1;
	switch (gameSize) 
	{
		case BIG:
			numColumns = GRID_COLUMNS_BIG;
			numRows = GRID_ROWS_BIG;
			break;
		case MEDIUM:
			numColumns = GRID_COLUMNS_MEDIUM;
			numRows = GRID_ROWS_MEDIUM;
			break;
		case SMALL:
			numColumns = GRID_COLUMNS_SMALL;
			numRows = GRID_ROWS_SMALL;
			break;
		default:
			break;
	}
	u32 clrGrey = GREY; 
	for (int i = 0; i < numColumns; i++)
		C2D_DrawRectSolid(TOP_SCREEN_WIDTH - i * TOP_SCREEN_WIDTH / numColumns, 0, 0, 1, SCREENS_HEIGHT, clrGrey);

	for (int i = 0; i < numRows; i++)
		C2D_DrawRectSolid(0, SCREENS_HEIGHT - i * SCREENS_HEIGHT / numRows, 0, TOP_SCREEN_WIDTH, 1, clrGrey);

}

int main(int argc, char* argv[])
{
	// Inits
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);	// Initialize citro3D (required for citro2D drawing)
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);	// Initialize citro2D
	C2D_Prepare();						// Prepares the GPU for rendering 2D content
	consoleInit(GFX_BOTTOM, NULL); 		// Provisionaly

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT); // The action of the game will occur here

	int gridEnabled = false;
	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		
		hidScanInput();
		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kDown & KEY_A) gridEnabled = !gridEnabled; // A turs the grid ON/OFF
		if (kDown & KEY_LEFT) // D-PAD LEFT changes the size of the grid
		{
			gameSize++;
			if(gameSize > VERY_BIG) gameSize = SMALL;
		}

		u32 backgrounfColor = WHITE;
		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, backgrounfColor);
		C2D_SceneBegin(top);

		if (gridEnabled) drawGrid();

		C3D_FrameEnd(0);
	}

	gfxExit();
	return 0;
}