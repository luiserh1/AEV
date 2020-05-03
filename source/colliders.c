#include <colliders.h>

bool getRectCollision(RectCollider *col1, RectCollider *col2)
{
	Coords coords1 = *col1->coords;
	Coords coords2 = *col2->coords;
	int distX = col1->area.sideX + col2->area.sideX;
	int distY = col1->area.sideY + col2->area.sideY;
	if (coords1.x > coords2.x)
    {
        if (coords1.y > coords2.y)
            return coords1.x - coords2.x < distX && coords1.y - coords2.y < distY;
        else 
            return coords1.x - coords2.x < distX && coords2.y - coords1.y < distY;
    }
    else
    {
        if (coords1.y > coords2.y)
            return coords2.x - coords1.x < distX && coords1.y - coords2.y < distY;
        else
            return coords2.x - coords1.x < distX && coords2.y - coords1.y < distY;
    }
}