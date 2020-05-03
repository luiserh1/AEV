#include <mapRepresentation.h>

int numRows;
int numColumns;
int gameSize;
int tileSide;
int currentPhase;

void decrementGameSize() { setGameFromSize(gameSize - 1); }

void incrementGameSize() { setGameFromSize(gameSize + 1); }

void nextPhase()
{
	if (++currentPhase > 9) { currentPhase = 0; }
}

void setGameFromSize(enum GAME_SIZE newSize)
{
	currentPhase = 0;
	gameSize = newSize;
	if(gameSize > BIG) gameSize = SMALL; // The size selection is cyclic
	else if(gameSize < SMALL) gameSize = BIG;
	int sideSize = GRID_SQUARE_SIDE_SMALL;
	for (int i = BIG; i > gameSize; i--) sideSize *= 2;
	numRows = SCREENS_HEIGHT / sideSize;
	numColumns = TOP_SCREEN_WIDTH / sideSize; 
	tileSide = sideSize;
}

MapRepresentation getEmptyMap()
{
	// Default values are ...
	// ... (PickableItem) { .item = NONE, .col = { .sideX = 0, .sideY = 0 } }
	// ... (TerrainObstacle) { .obs = EMPTY, .col = { .sideX = 0, .sideY = 0 } }
	// ... (Terrain) { .ter = SAND, .fatigue = 0 }
	MapRepresentation res;
	for (int i = 0; i < MAX_ROWS ; i++) res.terrain[i][MAX_COLUMNS - 1] = (Terrain) { .ter = WATER, .fatigue = -5 };
	return res;
}

void initMapFrames(MapRepresentation *mapRep, MapFrames *mapFrames)
{
	// Used to place the sprites with pivot in (0'5, 0'5)
	int tileOffset = tileSide / 2;

	// Ground images
	int sandSprite = terrain_Sand_Big_idx - gameSize;
	int waterSprite = terrain_Water_Big_idx - gameSize;
	// Obstacles sprites
	int cactusSprite = obstacles_Cactus_Big_idx - gameSize;
	// Item sprites
	int hatSprite = pickable_items_Hat_Chingon_Big_Above_idx - gameSize;

	for (int i = 0; i < numColumns; i++) 
	{
		for (int j = 0; j < numRows; j++)
		{
			// Ground
			int globalGridX = MAX_COLUMNS - i - 1;		// Theese coords represent the positions inside the
			int globalGridY = MAX_ROWS - j - 1;			// terrain elements (obstacles, itemes etc.) full matrixes
			int screenGridX = numColumns-i-1;			// And this coords represent a relative space extracted from that matrixes
			int screenGridY = numRows-j-1;				// where all positions may not be used depending oin the game size
			mapFrames->terrain[globalGridX][globalGridY][0].coords.x = screenGridX * tileSide;
			mapFrames->terrain[globalGridX][globalGridY][0].coords.y = screenGridY * tileSide;
			mapFrames->terrain[globalGridX][globalGridY][0].depth = 0.0f;
			C2D_Image *img = &mapFrames->terrain[globalGridX][globalGridY][0].img;
			switch (mapRep->terrain[globalGridX][globalGridY].ter)
			{
				case WATER:
					*img = C2D_SpriteSheetGetImage(terrainSpriteSheet, waterSprite);
					break;
				default: case SAND:
					*img = C2D_SpriteSheetGetImage(terrainSpriteSheet, sandSprite);
					break;
			}

			// Obstacles
			enum OBSTACLE obstacle = mapRep->obstacles[i][j].obs;
			if (obstacle != EMPTY)
			{
				C2D_Sprite *obsSprite = &mapFrames->obstacles[i][j][0].spr;
				switch (obstacle)
				{
					case CACTUS:
						C2D_SpriteFromSheet(obsSprite, obstaclesSpriteSheet, cactusSprite);
						break;
					default:
						break;
				}
				C2D_SpriteSetPos(obsSprite, i * tileSide + tileOffset, j * tileSide + tileOffset);
				C2D_SpriteSetCenter(obsSprite, 0.5f, 0.5f);
				C2D_SpriteSetDepth(obsSprite, 0.25f);
				
			}

			// Items
			enum ITEM item = mapRep->items[i][j].item;
			if (item != NONE)
			{
				C2D_Sprite *itemSprite = &mapFrames->items[i][j][0].spr;
				switch (item)
				{
					case CHEVERE_HAT:
						C2D_SpriteFromSheet(itemSprite, pickableItemsSpriteSheet, hatSprite);
						break;
					default:
						break;
				}
				C2D_SpriteSetPos(itemSprite, i * tileSide + tileOffset, j * tileSide + tileOffset);
				C2D_SpriteSetCenter(itemSprite, 0.5f, 0.5f);
				C2D_SpriteSetDepth(itemSprite, 0.25f);
			}
		}
	}
}

void initMapAnimation(MapFrames *mapFrames, MapAnimation *mapAnim)
{
	for (int i = 0; i < numColumns; i++) 
	{
		for (int j = 0; j < numRows; j++)
		{
			mapAnim->terrain[i][j].animComp.framesDurations = &standardFrameDurations[0];
			setUpImageAnimation(&mapAnim->terrain[i][j], &mapFrames->terrain[i][j][0], 1);
			resetAnimation(&mapAnim->terrain[i][j].animComp);

			mapAnim->obstacles[i][j].animComp.framesDurations = &standardFrameDurations[0];
			setUpSpriteAnimation(&mapAnim->obstacles[i][j], &mapFrames->obstacles[i][j][0], 1, true);
			resetAnimation(&mapAnim->obstacles[i][j].animComp);

			mapAnim->items[i][j].animComp.framesDurations = &standardFrameDurations[0];
			setUpSpriteAnimation(&mapAnim->items[i][j], &mapFrames->items[i][j][0], 1, true);
			resetAnimation(&mapAnim->items[i][j].animComp);
		}
	}
}

void renderMap(MapAnimation *mapAnim)
{
	for (int i = MAX_COLUMNS - 1; i > MAX_COLUMNS - numColumns - 1; i--) 
	{
		for (int j = MAX_ROWS - 1; j > MAX_ROWS - numRows - 1; j--)
		{
			playImageAnimation(&mapAnim->terrain[i][j], 1);
			playSpriteAnimation(&mapAnim->obstacles[i][j], 1);
			playSpriteAnimation(&mapAnim->items[i][j], 1);
		}
	}
}
