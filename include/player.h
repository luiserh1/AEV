#ifndef PLAYER_H
#define PLAYER_H

#include "mapRepresentation.h"
#include "frogEntity.h"

typedef struct 
{
	int livesCount;
	int fliesCount;
	bool chevereMode;
	bool buttonPressed;
	enum ITEM currentItem;
	Frog avatar;
} Player;

void resetPlayerProperties(Player *player);

void incrementFliesCount(Player *player);

void incrementLivesCount(Player *player);

void decrementLivesCount(Player *player);

void useItem(Player *player);

void renderPlayer(Player *player);

#endif