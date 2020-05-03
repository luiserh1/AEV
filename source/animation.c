#include <animation.h>

int standardFrameDurations[1024];

void initFramesDurations()
{
	// An animation using the standarFramDuration array as timer can contain 1024 frames
	// a fulla nimation of this kind would last, at a speed of 64 frames per second, 64 seconds
	//memset(standardFrameDurations, (int) 4, 1024*sizeof(standardFrameDurations[0]));
	for (int i = 0; i < 1024; i++) standardFrameDurations[i] = 5;
} 

void setUpSpriteAnimation(SpriteAnimation *anim, SpriteAnimationFrame *animFrames, int totalAnimFrames, bool setLooping)
{
	anim->animComp.totalFrames = totalAnimFrames;
	anim->animComp.loop = setLooping;
	anim->frames = animFrames;
	resetAnimation(&anim->animComp);
}

bool renderSpriteAnimation(SpriteAnimation *anim)
{
	if (anim->animComp.stopped) return false;
	int *frame = &anim->animComp.currentFrame;
	C2D_Sprite *sprite = &anim->frames[*frame].spr;
	C2D_DrawSprite(sprite);
	return true;
}

bool playSpriteAnimation(SpriteAnimation *anim, int frameSteps)
{
	if (anim->animComp.stopped) return false;
	if (!anim->animComp.paused) changeCurrentAnimationStep(&anim->animComp, frameSteps);
	renderSpriteAnimation(anim);
	return true;
}

Coords getSpriteAnimationCoords(SpriteAnimation *anim)
{
	C2D_Sprite *currentSprite = &anim->frames[anim->animComp.currentFrame].spr;
	Coords res;
	res.x = currentSprite->params.pos.x;
	res.y = currentSprite->params.pos.y;
	return res;
}

void setUpImageAnimation(ImageAnimation *anim, ImageAnimationFrame *animFrames, int totalAnimFrames)
{
	anim->animComp.totalFrames = totalAnimFrames;
	anim->animComp.loop = true;
	anim->frames = animFrames;
	resetAnimation(&anim->animComp);
}

bool renderImageAnimation(ImageAnimation *anim)
{
	if (anim->animComp.stopped) return false;
	int *frame = &anim->animComp.currentFrame;
	ImageAnimationFrame *imgFrame = &anim->frames[*frame];
	C2D_DrawImageAt(imgFrame->img, imgFrame->coords.x, imgFrame->coords.y, imgFrame->depth, imgFrame->tint, 1.0f, 1.0f);
	return true;
}

bool playImageAnimation(ImageAnimation *anim, int frameSteps)
{
	if (anim->animComp.stopped) return false;
	if (!anim->animComp.paused) changeCurrentAnimationStep(&anim->animComp, frameSteps);
	renderImageAnimation(anim);
	return true;
}

void setUpMovingObjectAnimation(MovingObjectAnimation *anim, SpriteAnimationFrame *frames, int totalFrames, bool setLooping)
{
	setUpSpriteAnimation(&anim->spriteAnim, frames, totalFrames, setLooping);
	anim->coords = getSpriteAnimationCoords(&anim->spriteAnim);
}

bool renderMovingObjectAnimation(MovingObjectAnimation *anim)
{
	return renderSpriteAnimation(&anim->spriteAnim);
}

bool playMovingObjectAnimation(MovingObjectAnimation *anim, int frameSteps)
{
	bool res = playSpriteAnimation(&anim->spriteAnim, frameSteps);
	anim->coords = getSpriteAnimationCoords(&anim->spriteAnim);
	return res;
}

void resetMovingObjectAnimation(MovingObjectAnimation *anim)
{
	resetAnimation(&anim->spriteAnim.animComp);
	anim->coords = getSpriteAnimationCoords(&anim->spriteAnim);
}

void resetAnimation(AnimationComponent *animComp)
{
	animComp->currentFrame = 0;
	animComp->currentStep = 0;
	animComp->animEnd = false;
	animComp->animStart = true;
	resumeAnimation(animComp);
}

void pauseAnimation(AnimationComponent *animComp) { animComp->paused = true; }

void resumeAnimation(AnimationComponent *animComp)
{ 
	animComp->paused = false;
	animComp->stopped = false;
}

void stopAnimation(AnimationComponent *animComp) {animComp->stopped = true; }

bool changeCurrentAnimationStep(AnimationComponent *animComp, int steps)
{
	if (animComp->stopped) return false;		// It's not possible to interact with stopped animations
	int *frame = &animComp->currentFrame;		
	int *step = &animComp->currentStep;
	*step += steps;
	if (steps != 0)	
	{
		animComp->animEnd = false;
		animComp->animStart = false;
	}													
	if (steps > 0)
	{		
		while (*step >= animComp->framesDurations[*frame])
		{
			*step -= animComp->framesDurations[*frame];
			(*frame)++;
			if (*frame >= animComp->totalFrames)
			{
				animComp->animEnd = true;
				if (!animComp->loop) 
				{
					animComp->paused = true;
					*frame = animComp->totalFrames-1;
					*step = animComp->framesDurations[*frame]-1;
				}
				else
				{
					*frame = 0;
					*step = 0;
				}
				break;
			}
		}
	}
	else // if (steps <= 0)
	{
		while (*step <= 0)
		{
			(*frame)--;
			*step += animComp->framesDurations[*frame];
			if (*frame < 0)
			{
				animComp->animStart = true;
				if (!animComp->loop) 
				{
					animComp->paused = true;
					*frame = 0;
					*step = 0;
				}
				else
				{
					*frame = animComp->totalFrames-1;
					*step = animComp->framesDurations[*frame]-1;
				}
				break;
			}
		}
	}
	return true;		
}
