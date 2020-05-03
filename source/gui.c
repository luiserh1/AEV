#include <gui.h>

// Bottom Screen animations for...
// ... the layout (images)
ImageAnimationFrame layoutFrames[1];
ImageAnimation layoutAnimation;
// ... the button (images)
ImageAnimationFrame buttonFrames[1];
ImageAnimation buttonAnimation;
ImageAnimationFrame buttonPressedFrames[1];
ImageAnimation buttonPressedAnimation;
// ... the life/death icons (sprites)
SpriteAnimationFrame lifeIconsFrames[3][1];
SpriteAnimation lifeIconsAnimations[3];
SpriteAnimationFrame deathIconsFrames[3][1];
SpriteAnimation deathIconsAnimations[3];
// ... the flies icons (sprites)
SpriteAnimationFrame flyIconsFrames[3][2];
SpriteAnimation flyIconsAnimations[3];
// ... the water barrel (sprites)
SpriteAnimationFrame barrelFrontFrames[1];
SpriteAnimation barrelFrontAnimation;
SpriteAnimationFrame barrelContentFrames[101];
SpriteAnimation barrelContentAnimation;
SpriteAnimationFrame barrelBackFrames[1];
SpriteAnimation barrelBackAnimation;
// ... the phase numbers (sprites)
SpriteAnimationFrame phaseNumbersSprites[9][20];
SpriteAnimation phaseNumbersAnimations[10];
// ... the items (sprites)
SpriteAnimationFrame itemsFrames[2][20];
SpriteAnimation itemsAnimations[2];


void initGUI()
{
	// Setting up the layout's animations
	layoutFrames[0].coords.x = 0;
	layoutFrames[0].coords.y = 0;
	layoutFrames[0].depth = -1.0f;
	C2D_Image *img = &layoutFrames[0].img;
	*img = C2D_SpriteSheetGetImage(menuImagesSpriteSheet, menu_images_Bottom_Screen_Menu_idx);
	layoutAnimation.animComp.framesDurations = &standardFrameDurations[0];
	setUpImageAnimation(&layoutAnimation, &layoutFrames[0], 1);

	// Setting up the life/death icons' animations
	for (int i = 0; i < 3; i++)
	{
		C2D_Sprite *spriteLife = &lifeIconsFrames[i][0].spr;
		C2D_Sprite *spriteDeath = &deathIconsFrames[i][0].spr;
		C2D_SpriteFromSheet(spriteLife, menuSpritesSpriteSheet, menu_sprites_Frog_Life_Icon_idx);
		C2D_SpriteFromSheet(spriteDeath, menuSpritesSpriteSheet, menu_sprites_Frog_Death_Icon_idx);
		C2D_SpriteSetCenter(spriteLife, 0.5f, 0.5f);
		C2D_SpriteSetCenter(spriteDeath, 0.5f, 0.5f);
		C2D_SpriteSetDepth(spriteLife, 0.0f);		
		C2D_SpriteSetDepth(spriteDeath, 0.0f);			
		C2D_SpriteSetPos(spriteLife, 270 - i * 40, 40);	
		C2D_SpriteSetPos(spriteDeath, 270 - i * 40, 40);
		lifeIconsAnimations[i].animComp.framesDurations = &standardFrameDurations[0];
		deathIconsAnimations[i].animComp.framesDurations = &standardFrameDurations[0];
		setUpSpriteAnimation(&lifeIconsAnimations[i], &lifeIconsFrames[i][0], 1, true);
		setUpSpriteAnimation(&deathIconsAnimations[i], &deathIconsFrames[i][0], 1, true);
	}

	// Setting up the button's animations
	buttonFrames[0].coords.x = 181;
	buttonFrames[0].coords.y = 177;
	layoutFrames[0].depth = 0.25f;
	img = &buttonFrames[0].img;
	*img = C2D_SpriteSheetGetImage(menuImagesSpriteSheet, menu_images_Button_Not_Pressed_idx);
	buttonAnimation.animComp.framesDurations = &standardFrameDurations[0];
	setUpImageAnimation(&buttonAnimation, &buttonFrames[0], 1);
	buttonPressedFrames[0].coords.x = 181;
	buttonPressedFrames[0].coords.y = 177;
	layoutFrames[0].depth = 0.25f;
	img = &buttonPressedFrames[0].img;
	buttonPressedAnimation.animComp.framesDurations = &standardFrameDurations[0];
	*img = C2D_SpriteSheetGetImage(menuImagesSpriteSheet, menu_images_Button_Pressed_idx);
	setUpImageAnimation(&buttonPressedAnimation, &buttonPressedFrames[0], 1);

	// Setting up the barrels's animations
	C2D_Sprite *sprite = &barrelBackFrames[0].spr;
	C2D_SpriteFromSheet(sprite, menuSpritesSpriteSheet, menu_sprites_Barrel_Inside_idx);
	C2D_SpriteSetCenter(sprite, 0.5f, 1.0f);
	C2D_SpriteSetDepth(sprite, 0.25f);			
	C2D_SpriteSetPos(sprite, 64, 201);
	barrelBackAnimation.animComp.framesDurations = &standardFrameDurations[0];
	setUpSpriteAnimation(&barrelBackAnimation, &barrelBackFrames[0], 1, true);
	sprite = &barrelFrontFrames[0].spr;
	C2D_SpriteFromSheet(sprite, menuSpritesSpriteSheet, menu_sprites_Barrel_Empty_idx);
	C2D_SpriteSetCenter(sprite, 0.5f, 1.0f);
	C2D_SpriteSetDepth(sprite, 0.75f);			
	C2D_SpriteSetPos(sprite, 64, 204);
	barrelFrontAnimation.animComp.framesDurations = &standardFrameDurations[0];
	setUpSpriteAnimation(&barrelFrontAnimation, &barrelFrontFrames[0], 1, true);
	for (int i = 0; i < 101; i++) 
	{
		C2D_Sprite *spriteContent = &barrelContentFrames[i].spr;
		C2D_SpriteFromSheet(spriteContent, menuSpritesSpriteSheet, menu_sprites_Barrel_Content_idx);
		C2D_SpriteSetCenter(spriteContent, 0.5f, 1.0f);
		C2D_SpriteSetDepth(spriteContent, 0.65f);			
		C2D_SpriteSetPos(spriteContent, 64, 198);	
		C2D_SpriteSetScale(spriteContent, 1.0f, 1.0f - 0.01f * i);
	}
	barrelContentAnimation.animComp.framesDurations = &standardFrameDurations[0];
	setUpSpriteAnimation(&barrelContentAnimation, &barrelContentFrames[0], 101, false);

	// Setting up the flies icons' animations
	for (int i = 0; i < 3; i++)
	{
		C2D_Sprite *spriteWingsUp = &flyIconsFrames[i][0].spr;
		C2D_Sprite *spriteWingsDown = &flyIconsFrames[i][1].spr;
		C2D_SpriteFromSheet(spriteWingsUp, menuSpritesSpriteSheet, menu_sprites_Fly_1_Big_idx);
		C2D_SpriteFromSheet(spriteWingsDown, menuSpritesSpriteSheet, menu_sprites_Fly_2_Big_idx);
		C2D_SpriteSetCenter(spriteWingsUp, 0.5f, 0.5f);
		C2D_SpriteSetCenter(spriteWingsDown, 0.5f, 0.5f);
		C2D_SpriteSetDepth(spriteWingsUp, 0.0f);		
		C2D_SpriteSetDepth(spriteWingsDown, 0.0f);			
		C2D_SpriteSetPos(spriteWingsUp, 125, 101 + 38 * i);	
		C2D_SpriteSetPos(spriteWingsDown,  125, 101 + 38 * i);
		flyIconsAnimations[i].animComp.framesDurations = &standardFrameDurations[0];
		setUpSpriteAnimation(&flyIconsAnimations[i], &flyIconsFrames[i][0], 2, true);
	}

	// Setting up the phase numbers animation
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			C2D_Sprite *sprite = &phaseNumbersSprites[i][j].spr;
			if (j < 10)
			{
				C2D_SpriteFromSheet(sprite, menuSpritesSpriteSheet, menu_sprites_Number_0_idx + i);
				C2D_SpriteSetScale(sprite, 1.0f - 0.1f * j, 1.0f - 0.1f * j);
			}
			else
			{
				int spriteID = menu_sprites_Number_1_idx + i;
				if (i == 9) spriteID = menu_sprites_Number_0_idx;
				C2D_SpriteFromSheet(sprite, menuSpritesSpriteSheet, spriteID);
				C2D_SpriteSetScale(sprite, 0.1f + 0.1f * (j - 10), 0.1f + 0.1f * (j -10));
			}
			C2D_SpriteSetPos(sprite, 143, 40);
			C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
			C2D_SpriteSetDepth(sprite, 0.0f);
		}
		phaseNumbersAnimations[i].animComp.framesDurations = &standardFrameDurations[0];
		setUpSpriteAnimation(&phaseNumbersAnimations[i], &phaseNumbersSprites[i][0], 20, false);
	}

	// Setting up the items animations
	for (int i = 1; i < 2; i++) // The item 0 is NONE
	{
		for (int j = 0; j < 20; j++)
		{
			C2D_Sprite *sprite = &itemsFrames[i][j].spr;
			C2D_SpriteFromSheet(sprite, itemsSpriteSheet, items_Hat_Chingon_Big_idx);
			C2D_SpriteSetPos(sprite, 230, 125);
			C2D_SpriteSetCenter(sprite, 0.5f, 0.5f);
			C2D_SpriteSetDepth(sprite, 0.0f);
			//C2D_SpriteSetRotationDegrees(sprite, q15ToFloat(sin1(FULL_TURN / 20 * j)) * 20.0f);
			C2D_SpriteSetRotationDegrees(sprite, q15ToFloat(sin1(floatToQ15(1.0f / 20 * j))) * 20.0f);
		}
		itemsAnimations[i].animComp.framesDurations = &standardFrameDurations[0];
		setUpSpriteAnimation(&itemsAnimations[i], &itemsFrames[i][0], 20, true);
	}
}

void displayGUI(Player *player)
{
	// The layout
	playImageAnimation(&layoutAnimation, 1);

	// The life and death icons
	for (int i = 0; i < 3; i++)
	{
		if (i < player->livesCount) playSpriteAnimation(&lifeIconsAnimations[i], 1);
		else playSpriteAnimation(&deathIconsAnimations[i], 1);
	}
	
	// The button
	if (player->buttonPressed) playImageAnimation(&buttonPressedAnimation, 1);
	else playImageAnimation(&buttonAnimation, 1);

	// The barrel and its content
	playSpriteAnimation(&barrelBackAnimation, 1);
	playSpriteAnimation(&barrelContentAnimation, 1);
	playSpriteAnimation(&barrelFrontAnimation, 1);


	for (int i = 0; i < player->fliesCount; i++)
		playSpriteAnimation(&flyIconsAnimations[i], 1);

	// The phase number
	playSpriteAnimation(&phaseNumbersAnimations[currentPhase], 1);

	// The item
	if (player->currentItem == NONE) return;
	playSpriteAnimation(&itemsAnimations[player->currentItem], 1);
}