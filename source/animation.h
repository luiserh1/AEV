#ifndef ANIMATION_H
#define ANIMATION_H

#include <citro2d.h>
#include <time.h>

typedef struct
{
	int x, y;
} Coords;

typedef struct 
{
	int duration, currentFrame;
	C2D_Sprite spr;
} SpriteAnimationFrame;

typedef struct
{
	int totalFrames, currentAnimationFrame;
	bool loop;
	bool paused, stopped;
	SpriteAnimationFrame *frames;
} SpriteAnimation;

typedef struct 
{
	Coords coords;
	float depth;
	int duration, currentFrame;
	C2D_Image img;
	C2D_ImageTint *tint;
} ImageAnimationFrame;

typedef struct
{
	bool paused, stopped;
	int totalFrames, currentAnimationFrame;
	ImageAnimationFrame *frames;
} ImageAnimation;

void setUpSpriteAnimation(SpriteAnimation *anim, SpriteAnimationFrame *frames, int totalFrames, bool setLooping);

void resetSpriteAnimation(SpriteAnimation *anim);

void pauseSpriteAnimation(SpriteAnimation *anim);

void playSpriteAnimation(SpriteAnimation *anim);

void stopSpriteAnimation(SpriteAnimation *anim);

bool renderSpriteAnimation(SpriteAnimation *anim, int frameSteps);

bool hasEndedSpriteAnimation(SpriteAnimation *anim);

Coords getCoordsFromSpriteAnimation(SpriteAnimation *anim);

void setUpImageAnimation(ImageAnimation *anim, ImageAnimationFrame *frames, int totalFrames);

void resetImageAnimation(ImageAnimation *anim);

void pauseImageAnimation(ImageAnimation *anim);

void playImageAnimation(ImageAnimation *anim);

void stopImageAnimation(ImageAnimation *anim);

bool renderImageAnimation(ImageAnimation *anim, int frameSteps);

#endif
