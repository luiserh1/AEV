#ifndef SPRITES_H
#define SPRITES_H

#include <menu_sprites.h>
#include <frog.h>
#include <items.h>
#include <pickable_items.h>
#include <menu_images.h>
#include <obstacles.h>
#include <particles.h>
#include <terrain.h>

#include <citro2d.h>

#include "geometry.h"

// The tex3ds spritesheets
extern C2D_SpriteSheet frogSpriteSheet;			// Sprites for the frog's representation
extern C2D_SpriteSheet itemsSpriteSheet;		// Sprites for the menu items' representation
extern C2D_SpriteSheet pickableItemsSpriteSheet;// Sprites for the pickable items' representation
extern C2D_SpriteSheet menuImagesSpriteSheet;	// Images for the menu's representation
extern C2D_SpriteSheet menuSpritesSpriteSheet;	// Sprites for the menu's representation
extern C2D_SpriteSheet obstaclesSpriteSheet;	// Sprites for the obstacles' representation
extern C2D_SpriteSheet particlesSpriteSheet;	// Sprites for the particles' representation
extern C2D_SpriteSheet terrainSpriteSheet; 		// Images for the level terrain's representation

void initSprites();

Rectangle getSpriteRectangle(C2D_Sprite spr);

#endif