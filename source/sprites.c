#include <sprites.h>

C2D_SpriteSheet frogSpriteSheet;			// Sprites for the frog's representation
C2D_SpriteSheet itemsSpriteSheet;			// Sprites for the menu items' representation
C2D_SpriteSheet pickableItemsSpriteSheet;	// Sprites for the pickable items' representation
C2D_SpriteSheet menuImagesSpriteSheet;		// Images for the menu's representation
C2D_SpriteSheet menuSpritesSpriteSheet;		// Sprites for the menu's representation
C2D_SpriteSheet obstaclesSpriteSheet;		// Sprites for the obstacles' representation
C2D_SpriteSheet particlesSpriteSheet;		// Sprites for the particles' representation
C2D_SpriteSheet terrainSpriteSheet; 		// Images for the level terrain's representation

void initSprites()
{
	frogSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/frog.t3x");
	itemsSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/items.t3x");
	pickableItemsSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/pickable_items.t3x");
	menuImagesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/menu_images.t3x");
	menuSpritesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/menu_sprites.t3x");
	obstaclesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/obstacles.t3x");
	particlesSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/particles.t3x");
	terrainSpriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/terrain.t3x");
	if (!frogSpriteSheet || !itemsSpriteSheet || !pickableItemsSpriteSheet || !menuImagesSpriteSheet || !menuSpritesSpriteSheet
		|| !obstaclesSpriteSheet || !particlesSpriteSheet || !terrainSpriteSheet) svcBreak(USERBREAK_PANIC);
}

Rectangle getSpriteRectangle(C2D_Sprite spr)
{
	return (Rectangle) { .sideX = spr.params.pos.w, .sideY = spr.params.pos.h };
}