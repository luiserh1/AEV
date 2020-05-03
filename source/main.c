#define DEBBUG_CONTROLS true

#include <citro2d.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <time.h>

#include <colors.h>
#include <sin1.h> // author stfwi from https://www.atwillys.de/content/cc/sine-lookup-for-embedded-in-c/?lang=en
#include <intArithmetic.h>
#include <animation.h>
#include <frog.h>
#include <player.h>
#include <sprites.h>
#include <mapRepresentation.h>
#include <gui.h>
#include <debug.h>

// Q15 number representing a full turn (360 degrees / 2*PI radians)
#define FULL_TURN 32766

void initGame();
void drawGrid();
void incrementSize();
void decrementSize();
void processGameplayControls(u32 kDown, u32 kHeld, touchPosition touch);
void processDebugControls(u32 kDown);
void showBottomScreen(C3D_RenderTarget* bot);
void showTopScreen(C3D_RenderTarget* top);
void showConsole();

static bool gridEnabled = false; // This is a debug option to see all of the tiles

static Player player;						// The player
static MapRepresentation currentMapRep;		// The map representative structure
static MapFrames currentMapFrames;			// The map frames for...
static MapAnimation currentMapAnim;			// The map animations

static bool consoleMode;			// In console mode it's possible to visualize more data
									// Once you active it there is not going back (debug only)

//////////////////////
// SET UP FUNCTIONS //
//////////////////////

void initGame()
{
	// The order DOES matter
	romfsInit();						// Requred to filis input
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);	// Initialize citro3D (required for citro2D drawing)
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);	// Initialize citro2D
	C2D_Prepare();						// Prepares the GPU for rendering 2D content

	initSprites();
	setGameFromSize(SMALL);
	initFramesDurations();
	currentMapRep = getEmptyMap();
	initMapFrames(&currentMapRep, &currentMapFrames);
	initMapAnimation(&currentMapFrames, &currentMapAnim);
	resetPlayerProperties(&player);
	initGUI();

	consoleMode = false;
}

////////////////////////
// GAMPLAY PROCESSING //
////////////////////////

void incrementSize()
{
	incrementGameSize();
	resetPlayerProperties(&player);
}

void decrementSize()
{
	decrementGameSize();
	resetPlayerProperties(&player);
}

void processDebugControls(u32 kDown)
{
	if (kDown & KEY_A) gridEnabled = !gridEnabled; // A turns the grid ON/OFF
	if (kDown & KEY_R) // R and L buttons change the size of the grid
		incrementSize();
	if (kDown & KEY_L)
		decrementSize();
	if (kDown & KEY_ZL)
		incrementFliesCount(&player);
	if (kDown & KEY_ZR)
		nextPhase();
	if (kDown & KEY_X) player.currentItem = CHEVERE_HAT;
	if (kDown & KEY_Y && !consoleMode) showConsole();
}

void processGameplay(u32 kDown, u32 kHeld, touchPosition touch)
{
	bool animationEnd = player.avatar.anim->spriteAnim.animComp.animEnd;

	if (animationEnd)
	{
		if (kDown & KEY_LEFT)
			frogDoMove(&player.avatar, LEFT);
		else if (kDown & KEY_RIGHT)
			frogDoMove(&player.avatar, RIGHT);
		else if (kDown & KEY_DOWN)
			frogDoMove(&player.avatar, DOWN);
		else if (kDown & KEY_UP)
			frogDoMove(&player.avatar, UP);
		else frogDoMove(&player.avatar, IRRELEVANT);
	}

	// You can use the item pressing the button on the bottom screen or hitting B
	if(kHeld & KEY_B || (touch.px > 181 && touch.px < 278 && touch.py > 177 &&  touch.py < 207)) 
	{
		if (!player.buttonPressed) player.buttonPressed = true;
	}
	else if (player.buttonPressed)
	{
		player.buttonPressed = false;
		useItem(&player);
	}
		
}

///////////////////////
// DRAWING FUNCTIONS //
///////////////////////

void drawGrid() // Using rectangles to draw lines might not be the most efficient way to go,
{				// but since it's GPU work I guess it's better than going pixel by pixel
	u32 gridClr = RED; 
	for (int i = 0; i < numRows; i++)
	{
		C2D_DrawRectSolid(0, SCREENS_HEIGHT - i * tileSide, 1.0f, TOP_SCREEN_WIDTH, 1, gridClr);
	}
	for (int i = 0; i < numColumns; i++)
	{
		C2D_DrawRectSolid(TOP_SCREEN_WIDTH - i * tileSide, 1.0f, 0, 1, SCREENS_HEIGHT, gridClr);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

void showBottomScreen(C3D_RenderTarget* bot)
{
	u32 backgroundBot = BLUE;
	C2D_TargetClear(bot, backgroundBot);
	//C3D_FrameDrawOn(bot); I do not know what is this for, but it seems like I do not need it
	C2D_SceneBegin(bot);
	displayGUI(&player);
}

void showTopScreen(C3D_RenderTarget* top)
{
	u32 backgroundTop = BLUE;
	C2D_TargetClear(top, backgroundTop);
	C2D_SceneBegin(top);

	// The player
	renderPlayer(&player);

	// The map
	//renderMap(&currentMapAnim);

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
	initGame();

	C3D_RenderTarget *top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT); // The action of the game will occur here
	C3D_RenderTarget *bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT); 
	
	// Main loop
	while (aptMainLoop())
	{	
		if (consoleMode)
		{
			consoleClear();
			// Debug area
			printf("Game Size: %.10s\n", gameSizeNames[gameSize]);
			printf("State: %.10s\n", stateNames[player.avatar.state]);
			printf("Orientation: %.10s\n", orientationNames[player.avatar.orientation]);
			printf("Current Frame(%d/%d)\nFrame Process(%d/%d)\nAnimation end?:%d\n",
				player.avatar.anim->spriteAnim.animComp.currentFrame + 1, player.avatar.anim->spriteAnim.animComp.totalFrames,
				player.avatar.anim->spriteAnim.animComp.currentStep,
				player.avatar.anim->spriteAnim.animComp.framesDurations[player.avatar.anim->spriteAnim.animComp.currentFrame],
				player.avatar.anim->spriteAnim.animComp.animEnd);
			Coords coordsScene = *player.avatar.screenCoords;
			Coords coordsGrid = player.avatar.gridCoords;
			Coords coordsAnimationGetter = getSpriteAnimationCoords(&player.avatar.anim->spriteAnim);
			Coords coordsHat = getSpriteAnimationCoords(&player.avatar.animations.hat);
			Coords coordsAnimationMov = player.avatar.anim->coords;
			printf("PlayerScreenCoords(%d, %d)\n", coordsScene.x, coordsScene.y);
			printf("PlayerAnimationGetterCoords(%d, %d)\n", coordsAnimationGetter.x, coordsAnimationGetter.y);
			printf("PlayerAnimationCoords(%d, %d)\n", coordsAnimationMov.x, coordsAnimationMov.y);
			printf("HatCoords(%d, %d)\n", coordsHat.x, coordsHat.y);
			printf("GridCoords(%d, %d)\n", coordsGrid.x, coordsGrid.y);
			printf("\n========================================\nLives: %d\n", player.livesCount);
			printf("Flies: %d\n", player.fliesCount);
			printf("Current Item: %.10s\n", itemNames[player.currentItem]);
			printf("Chingon Mode: %d\n", player.chevereMode);
			printf("Capacity: %.2f\n", 
				(barrelContentAnimation.animComp.totalFrames - barrelContentAnimation.animComp.currentFrame - 1)
				/ (float)barrelContentAnimation.animComp.totalFrames);
			changeCurrentAnimationStep(&barrelContentAnimation.animComp, 1);
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