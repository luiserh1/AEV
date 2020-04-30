#include "animation.h"


void setUpSpriteAnimation(SpriteAnimation *anim, SpriteAnimationFrame *animFrames, int totalAnimFrames, bool setLooping)
{
	anim->totalFrames = totalAnimFrames;
	anim->loop = setLooping;
	anim->frames = animFrames;
	resetSpriteAnimation(anim);
}

void resetSpriteAnimation(SpriteAnimation *anim)
{
	anim->currentAnimationFrame = 0;
	anim->endFrame = false;
	for (int i = 0; i < anim->totalFrames; i++)
		anim->frames[i].currentFrame = 0;
	playSpriteAnimation(anim);
}

void pauseSpriteAnimation(SpriteAnimation *anim) { anim->paused = true; }

void playSpriteAnimation(SpriteAnimation *anim)
{ 
	anim->paused = false;
	anim->stopped = false;
}

void stopSpriteAnimation(SpriteAnimation *anim) { anim->stopped = true; }

bool renderSpriteAnimation(SpriteAnimation *anim, int frameSteps)
{
	if (anim->stopped) return false;				// If it's stopped it does not show anything
	int *caf = &anim->currentAnimationFrame;		
	int *cff = &anim->frames[*caf].currentFrame;
	if (*caf >= anim->totalFrames)					// If it ended last frame and it's a loop, restarts
		resetSpriteAnimation(anim);					// If it's not a loop it will display the last frame each time
	C2D_Sprite *sprite = &anim->frames[*caf].spr;	// as it was paused there
	C2D_DrawSprite(sprite);
	if (anim->paused) return false;
	*cff += frameSteps;
	while (*cff >= anim->frames[*caf].duration)
	{
		*cff -= anim->frames[*caf].duration;
		(*caf)++;
		if (*caf >= anim->totalFrames)
		{
			if (!anim->loop) 
			{
				anim->paused = true;
				(*caf)--;
			}
			anim->endFrame = true;
			break;
		}
	}
	return anim->endFrame;
}

bool hasEndedSpriteAnimation(SpriteAnimation *anim) { return anim->endFrame; }

Coords getCoordsFromSpriteAnimation(SpriteAnimation *anim)
{																		// The "-1" means the counter goes one
	C2D_Sprite *currentSprite = &anim->frames[anim->currentAnimationFrame - 1].spr;	// frame ahead
	Coords res;
	res.x = currentSprite->params.pos.x;
	res.y = currentSprite->params.pos.y;
	return res;
}

void setUpImageAnimation(ImageAnimation *anim, ImageAnimationFrame *animFrames, int totalAnimFrames)
{
	anim->totalFrames = totalAnimFrames;
	anim->frames = animFrames;
	resetImageAnimation(anim);
}

void resetImageAnimation(ImageAnimation *anim)
{
	anim->currentAnimationFrame = 0;
	for (int i = 0; i < anim->totalFrames; i++)
		anim->frames[i].currentFrame = 0;
	playImageAnimation(anim);
}

void pauseImageAnimation(ImageAnimation *anim) { anim->paused = true; }

void playImageAnimation(ImageAnimation *anim)
{ 
	anim->paused = false;
	anim->stopped = false;
}

bool renderImageAnimation(ImageAnimation *anim, int frameSteps)
{
	if (anim->stopped) return false;
	int *caf = &anim->currentAnimationFrame;
	if (*caf >= anim->totalFrames)					
		resetImageAnimation(anim);
	ImageAnimationFrame *imgFrame = &anim->frames[*caf];
	int *cff = &imgFrame->currentFrame;	
	C2D_DrawImageAt(imgFrame->img, imgFrame->coords.x, imgFrame->coords.y, imgFrame->depth, imgFrame->tint, 1.0f, 1.0f);
	if (anim->paused) return false;
	*cff += frameSteps;
	while (*cff >= anim->frames[*caf].duration)
	{
		*cff -= anim->frames[*caf].duration;
		(*caf)++;
		if (*caf >= anim->totalFrames)
		{
			return true;
		}
	}
	return false;
}

void stopImageAnimation(ImageAnimation *anim) { anim->stopped = true; }
