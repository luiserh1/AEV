#ifndef ANIMATION_H
#define ANIMATION_H

// Time per animation frame
#define TIME_PER_FRAME_MILIS 16;

#include <citro2d.h>
#include <time.h>

#include "geometry.h"

typedef struct
{
	int totalFrames, currentFrame;
	bool loop;
	bool animStart, animEnd;
	bool paused, stopped;
	int currentStep;
	int *framesDurations;
} AnimationComponent;

typedef struct 
{
	C2D_Sprite spr;
} SpriteAnimationFrame;

typedef struct // TODO change the structure to use C2D_DrawParams
{
	Coords coords;
	float depth;
	C2D_Image img;
	C2D_ImageTint *tint;
} ImageAnimationFrame;

typedef struct
{
	AnimationComponent animComp;
	SpriteAnimationFrame *frames;
} SpriteAnimation;

typedef struct
{
	AnimationComponent animComp;
	ImageAnimationFrame *frames;
} ImageAnimation;

typedef struct
{
	SpriteAnimation spriteAnim;
	Coords coords;
} MovingObjectAnimation;

extern int standardFrameDurations[1024];

void initFramesDurations();

void setUpSpriteAnimation(SpriteAnimation *anim, SpriteAnimationFrame *frames, int totalFrames, bool setLooping);

bool renderSpriteAnimation(SpriteAnimation *anim);

bool playSpriteAnimation(SpriteAnimation *anim, int frameSteps);

Coords getSpriteAnimationCoords(SpriteAnimation *anim);

void setUpImageAnimation(ImageAnimation *anim, ImageAnimationFrame *frames, int totalFrames);

bool renderImageAnimation(ImageAnimation *anim);

bool playImageAnimation(ImageAnimation *anim, int frameSteps);

void setUpMovingObjectAnimation(MovingObjectAnimation *anim, SpriteAnimationFrame *frames, int totalFrames, bool setLooping);

bool renderMovingObjectAnimation(MovingObjectAnimation *anim);

bool playMovingObjectAnimation(MovingObjectAnimation *anim, int frameSteps);

void resetMovingObjectAnimation(MovingObjectAnimation *anim);

void resetAnimation(AnimationComponent *animComp);

void pauseAnimation(AnimationComponent *animComp); // TODO check if this functions can be inline

void resumeAnimation(AnimationComponent *animComp);

void stopAnimation(AnimationComponent *animComp);

bool changeCurrentAnimationStep(AnimationComponent *animComp, int steps);

#endif
