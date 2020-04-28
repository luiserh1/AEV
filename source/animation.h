#ifndef ANIMATION_H
#define ANIMATION_H

#include <citro2d.h>

typedef struct 
{
	int timeFrames;
	C2D_Sprite spr;
} AnimationFrame;

typedef struct
{
	int fullAnimationFrames;
	int currentAnimationFrame;
	bool loop;
	bool playing;
	AnimationFrame *frames;
} Animation;

void setUpeAnimation(Animation *anim, AnimationFrame *frames, bool setLooping);

void resetAnimation(Animation *anim);

void stopAnimation(Animation *anim);

void playAnimation(Animation *anim);

#endif
