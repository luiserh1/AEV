#include <frogEntity.h>

void resetFrog(Frog *frog)
{
	int tileOffset = tileSide / 2;

	initFrogFrames(gameSize, &frog->frames);
	initFrogAnimation(&frog->frames, &frog->animations);

	for (int i = 0; i < frog->animations.idle.spriteAnim.animComp.totalFrames; i++)
	{
		C2D_SpriteSetPos(&frog->animations.idle.spriteAnim.frames[i].spr,
			TOP_SCREEN_WIDTH 	- (numColumns) 			* tileSide + tileOffset,
			SCREENS_HEIGHT 		- (numRows / 2 + 1) 	* tileSide + tileOffset);
	}

	for (int i = 0; i < frog->animations.hat.animComp.totalFrames; i++)
	{
		C2D_SpriteSetPos(&frog->animations.hat.frames[i].spr,
			TOP_SCREEN_WIDTH 	- (numColumns) 			* tileSide + tileOffset,
			SCREENS_HEIGHT 		- (numRows / 2 + 1) 	* tileSide + tileOffset);
	}
	
	frog->anim = &frog->animations.idle;
	resetMovingObjectAnimation(frog->anim);
	resetAnimation(&frog->animations.hat.animComp);
	frog->screenCoords = &frog->anim->coords;
	frog->state = IDLE;
	frog->col->coords = frog->screenCoords; // TODO -> double indirection is required
	frog->col->area.sideX = frog->anim->spriteAnim.frames[frog->anim->spriteAnim.animComp.currentFrame].spr.params.pos.w;
	frog->col->area.sideY = frog->anim->spriteAnim.frames[frog->anim->spriteAnim.animComp.currentFrame].spr.params.pos.h;
	frog->gridCoords.x = 0;
	frog->gridCoords.y = numRows / 2;
	if (numColumns % 2 == 0) frog->gridCoords.y--;
	setFrogOrientation(frog, RIGHT);
}

void initFrogFrames(enum GAME_SIZE gameSize, FrogFrames *frogFrames)
{
	// Idle
	int totalFramesIdle = 1; // TODO change this magical numbers
	for (int i = 0; i < totalFramesIdle; i++)
	{
		C2D_Sprite *spriteIdle = &frogFrames->idle[i].spr;
		C2D_SpriteFromSheet(spriteIdle, frogSpriteSheet, frog_Frog_Standing_Big_idx - gameSize); // IMPORTANT they are one after each other on the spritesheet
		C2D_SpriteSetCenter(spriteIdle, 0.5f, 0.5f);
		C2D_SpriteSetDepth(spriteIdle, 0.75f);
	}

	// Jumping
	int totalFramesJumping = 30;
	for (int i = 0; i < totalFramesJumping; i++) 
	{
		C2D_Sprite *spriteJumping = &frogFrames->jumping[i].spr;
		if (i > 3 && i < 27) C2D_SpriteFromSheet(spriteJumping, frogSpriteSheet, frog_Frog_Jumping_Big_idx - gameSize); // Since we do not have many sprites the first
		else C2D_SpriteFromSheet(spriteJumping, frogSpriteSheet, frog_Frog_Standing_Big_idx - gameSize); 			// and last sprite of the jumping animation are
		C2D_SpriteSetCenter(spriteJumping, 0.5f, 0.5f);																// just the frog standing sprites.
		C2D_SpriteSetDepth(spriteJumping, 0.75f);																	// TODO add more and different sprites
	}

	// The wereable and chevere hat
	int totalFramesHat = 30;
	for (int i = 0; i < totalFramesHat; i++) 
	{
		C2D_Sprite *hatSprite = &frogFrames->hat[i].spr;
		C2D_SpriteFromSheet(hatSprite, frogSpriteSheet, frog_Hat_Chingon_Big_Above_idx - gameSize);
		C2D_SpriteSetCenter(hatSprite, 0.5f, 0.5f);										
		C2D_SpriteSetDepth(hatSprite, 0.85f);
	}
}

void initFrogAnimation(FrogFrames *frogFrames, FrogAnimation *frogAnim)
{
	frogAnim->idle.spriteAnim.animComp.framesDurations = &standardFrameDurations[0];
	setUpMovingObjectAnimation(&frogAnim->idle, &frogFrames->idle[0], 1, false);
	resetMovingObjectAnimation(&frogAnim->idle);

	frogAnim->jumping.spriteAnim.animComp.framesDurations = &standardFrameDurations[0];
	setUpMovingObjectAnimation(&frogAnim->jumping, &frogFrames->jumping[0], 30, false);
	resetMovingObjectAnimation(&frogAnim->jumping);

	frogAnim->hat.animComp.framesDurations = &standardFrameDurations[0];
	setUpSpriteAnimation(&frogAnim->hat, &frogFrames->hat[0], 30, false);
	resetAnimation(&frogAnim->hat.animComp);
}

void setFrogOrientation(Frog *frog, enum ORIENTATION orientation)
{
	if (orientation == IRRELEVANT) return;
	frog->orientation = orientation;
	float degrees = 0;
	switch (orientation)
	{
		case RIGHT: degrees = 90; break;
		case DOWN: degrees = 180; break;
		case LEFT: degrees = 270; break;
		default: case UP: break;
	}
	for (int i = 0; i < frog->anim->spriteAnim.animComp.totalFrames; i++)
		C2D_SpriteSetRotationDegrees(&frog->anim->spriteAnim.frames[i].spr, degrees);
}

void frogDoIdle(Frog *frog)
{
	frog->state = IDLE;
	int totalFramesIdle = frog->animations.idle.spriteAnim.animComp.totalFrames;
	for (int i = 0; i < totalFramesIdle; i++)
	{
		C2D_SpriteSetPos(&frog->animations.idle.spriteAnim.frames[i].spr, frog->screenCoords->x, frog->screenCoords->y);
	}
	Coords hatCoords = *frog->screenCoords;
	int totalFramesHat = frog->animations.hat.animComp.totalFrames;
	for (int i = 0; i < totalFramesHat; i++)
	{
		C2D_SpriteSetPos(&frog->animations.hat.frames[i].spr, hatCoords.x, hatCoords.y); 
	}
	frog->anim = &frog->animations.idle;
	frog->screenCoords = &frog->anim->coords;
}

void frogDoJump(Frog *frog, enum ORIENTATION orientation)
{
	frog->state = JUMPING;
	int deltaX = (orientation == RIGHT) * tileSide - (orientation == LEFT) * tileSide;
	int deltaY = (orientation == DOWN) * tileSide - (orientation == UP) * tileSide; // The y axis are upside down

	frog->gridCoords.x += (orientation == RIGHT) - (orientation == LEFT);
	frog->gridCoords.y += (orientation == DOWN) - (orientation == UP);

	Coords oldCoords = *frog->screenCoords;
	
	int totalFramesJumping = frog->animations.jumping.spriteAnim.animComp.totalFrames; 
	for (int i = 0; i < totalFramesJumping - 1; i++)
	{
		C2D_SpriteSetPos(&frog->animations.jumping.spriteAnim.frames[i].spr,
			oldCoords.x + deltaX * q15ToFloat(sin1(floatToQ15(1.0f / 4 * i / totalFramesJumping))),
			oldCoords.y + deltaY * q15ToFloat(sin1(floatToQ15(1.0f / 4 * i / totalFramesJumping))));
	}
	C2D_SpriteSetPos(&frog->animations.jumping.spriteAnim.frames[totalFramesJumping - 1].spr,
			oldCoords.x + deltaX, oldCoords.y + deltaY); // To ensure the distance
	int totalFramesHat = frog->animations.hat.animComp.totalFrames; 
	for (int i = 0; i < totalFramesHat - 1; i++)
	{
		C2D_SpriteSetPos(&frog->animations.hat.frames[i].spr,
			oldCoords.x + deltaX * q15ToFloat(sin1(floatToQ15(1.0f / 4 * i / totalFramesHat))),
			oldCoords.y + deltaY * q15ToFloat(sin1(floatToQ15(1.0f / 4 * i / totalFramesHat))));
	}
	C2D_SpriteSetPos(&frog->animations.hat.frames[totalFramesHat - 1].spr,
			oldCoords.x + deltaX, oldCoords.y + deltaY);
	frog->anim = &frog->animations.jumping;
	frog->screenCoords = &frog->anim->coords;
}

void frogDoMove(Frog *frog, enum ORIENTATION orientation)
{
	bool movement = true;
	if (frog->state == DYING || frog->state == SPAWNING) return;
	if (orientation == IRRELEVANT)
		movement = false;
	else
	{
		if (orientation != frog->orientation) movement = false;
		else 
		{
			if   (	(frog->gridCoords.x < 1 && (orientation == LEFT))
				|| 	(frog->gridCoords.x > numColumns - 2 && (orientation == RIGHT))
				|| 	(frog->gridCoords.y < 1 && (orientation == UP))
				|| 	(frog->gridCoords.y > numRows - 2 && (orientation == DOWN)))
				{ frogDoJump(frog, IRRELEVANT); }
			else { frogDoJump(frog, orientation); }
		}
	}
	if (!movement) frogDoIdle(frog);
	resetAnimation(&frog->animations.hat.animComp);

	resetMovingObjectAnimation(frog->anim);
	setFrogOrientation(frog, orientation);
}