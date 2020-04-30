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
	if (anim->stopped) return false;
	int *caf = &anim->currentAnimationFrame;
	int *cff = &anim->frames[*caf].currentFrame;
	C2D_Sprite *sprite = &anim->frames[*caf].spr;
	C2D_DrawSprite(sprite);
	if (anim->paused) return false;
	*cff += frameSteps;
	if (*cff >= anim->frames[*caf].duration)
	{
		*cff = 0;
		(*caf)++;
		if (*caf >= anim->totalFrames)
		{
			if (!anim->loop) // If it's not a loop the animation remains paused in it's last frame
			{
				anim->paused = true;
				(*caf)--;
			}
			else *caf = 0; // If it's a loop the animation starts again
			return true;
		}
	}
	return false;
}

bool hasEndedSpriteAnimation(SpriteAnimation *anim)
{
	int *caf = &anim->currentAnimationFrame;
	int *cff = &anim->frames[*caf].currentFrame;
	if (*caf+1 >= anim->totalFrames && *cff+1 >= anim->frames[*caf].duration) return true;
	else return false;
}

Coords getCoordsFromSpriteAnimation(SpriteAnimation *anim)
{
	C2D_Sprite *currentSprite = &anim->frames[anim->currentAnimationFrame].spr;
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
	ImageAnimationFrame *imgFrame = &anim->frames[*caf];
	int *cff = &imgFrame->currentFrame;
	C2D_DrawImageAt(imgFrame->img, imgFrame->coords.x, imgFrame->coords.y, imgFrame->depth, imgFrame->tint, 1.0f, 1.0f);
	if (anim->paused) return false;
	*cff += frameSteps;
	if (*cff >= anim->frames[*caf].duration)
	{
		*cff = 0;
		(*caf)++;
		if (*caf >= anim->totalFrames) {
			*caf = 0;
			return true;
		}
	}
	return false;
}

void stopImageAnimation(ImageAnimation *anim) { anim->stopped = true; }
