#ifndef COLLIDERS_H
#define COLLIDERS_H

#include "geometry.h"
#include <stdbool.h>

typedef struct 
{
	Coords *coords;
	Rectangle area;
} RectCollider;

extern bool getRectCollision(RectCollider *col1, RectCollider *col2);

#endif