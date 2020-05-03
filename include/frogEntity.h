#ifndef FROGENTITY_H
#define FROGENTITY_H

#include <citro2d.h>

#include "intArithmetic.h"
#include "sin1.h"
#include "mapRepresentation.h"
#include "colliders.h"
#include "animation.h"

enum STATE { SPAWNING, IDLE, JUMPING, DYING };
enum ORIENTATION { UP, DOWN, RIGHT, LEFT, IRRELEVANT };


typedef struct
{
	MovingObjectAnimation idle;
	MovingObjectAnimation jumping;
	SpriteAnimation hat;
} FrogAnimation;

typedef struct
{
	SpriteAnimationFrame idle[1];
	SpriteAnimationFrame jumping[30];
	SpriteAnimationFrame hat[30];
} FrogFrames;

typedef struct 
{
	RectCollider *col;					// The collider
	Coords *screenCoords;				// Coords to represent the position on the screen
	Coords gridCoords;					// Coords to represent the position on the grid
	enum STATE state;					// State of the frog's state machine
	enum ORIENTATION orientation;		// Orientation wich the frog is facing
	MovingObjectAnimation *anim;		// Sprite to draw the frog  with coords
	FrogAnimation animations;			// The different animations of the frog
	FrogFrames frames;					// And its frames
} Frog;


extern RectCollider idleColliders[4];
extern RectCollider jumpColliders[4];

void resetFrog(Frog *frog);

void initFrogFrames(enum GAME_SIZE gameSize, FrogFrames *frogFrames);

void initFrogAnimation(FrogFrames *frogFrames, FrogAnimation *frogAnim);

void setFrogOrientation(Frog *frog, enum ORIENTATION orientation);

void frogDoIdle(Frog *frog);

void frogDoJump(Frog *frog, enum ORIENTATION orientation);

void frogDoMove(Frog *frog, enum ORIENTATION orientation);

#endif