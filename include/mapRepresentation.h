#ifndef MAPREPRESENTATION_H
#define MAPREPRESENTATION_H

#include "colliders.h"
#include "geometry.h"
#include "animation.h"
#include "sprites.h"

// Screen size macros
#define TOP_SCREEN_WIDTH  400
#define BOTTOM_SCREEN_WIDTH  320
#define SCREENS_HEIGHT 240
// Tile size macros
#define MAX_ROWS 12
#define MAX_COLUMNS 20
#define GRID_SQUARE_SIDE_SMALL 20

enum GAME_SIZE { SMALL, MEDIUM, BIG };
enum ITEM { NONE, CHEVERE_HAT };
enum OBSTACLE { EMPTY, CACTUS };
enum TERRAIN { SAND, WATER };

typedef struct
{
	enum ITEM item;
	RectCollider col;
} PickableItem;

typedef struct
{
	enum OBSTACLE obs;
	RectCollider col;
} TerrainObstacle;

typedef struct
{
	enum TERRAIN ter;
	int fatigue;
} Terrain;

typedef struct
{
	PickableItem items[MAX_ROWS][MAX_COLUMNS];
	TerrainObstacle obstacles[MAX_ROWS][MAX_COLUMNS];
	Terrain terrain[MAX_ROWS][MAX_COLUMNS];
} MapRepresentation;

typedef struct
{
	SpriteAnimation items[MAX_ROWS][MAX_COLUMNS];
	SpriteAnimation obstacles[MAX_ROWS][MAX_COLUMNS];
	ImageAnimation terrain[MAX_ROWS][MAX_COLUMNS];
} MapAnimation;

typedef struct
{
	SpriteAnimationFrame items[MAX_ROWS][MAX_COLUMNS][1];
	SpriteAnimationFrame obstacles[MAX_ROWS][MAX_COLUMNS][1];
	ImageAnimationFrame terrain[MAX_ROWS][MAX_COLUMNS][1];
} MapFrames;

extern int numRows, numColumns;	// This variables will contain the values to know how many tiles are
extern int gameSize;			// Each level may have a different number of tiles
extern int tileSide;			// The size in pixels of a tile of the map
extern int currentPhase;		// Number corresponding to the current phase

void decrementGameSize();

void incrementGameSize();

void nextPhase();

void setGameFromSize(enum GAME_SIZE newSize);

MapRepresentation getEmptyMap();

void initMapFrames(MapRepresentation *mapRep, MapFrames *mapFrames);

void initMapAnimation(MapFrames *mapFrames, MapAnimation *mapAnim);

void renderMap(MapAnimation *mapAnim);

#endif