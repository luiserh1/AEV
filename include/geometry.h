#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct
{
	int x, y;
} Coords;

typedef struct
{
	int sideX, sideY;
} Rectangle;

inline void rotateRectangle(Rectangle *rect)
{
	int auX = rect->sideX;
	rect->sideX = rect->sideY;
	rect->sideY = auX;
}

#endif