#include <player.h>

void resetPlayerProperties(Player *player)
{
	player->buttonPressed = false;
	player->chevereMode = false;
	player->currentItem = NONE;
	player->livesCount = 1;
	player->fliesCount = 1;
	resetFrog(&player->avatar);
}

void incrementFliesCount(Player *player)
{
	if (++(player->fliesCount) > 3)
	{ 
		player->fliesCount = 0;
		incrementLivesCount(player);
	}
}

void incrementLivesCount(Player *player)
{
	if (player->livesCount < 3) { player->livesCount++; }
}

void decrementLivesCount(Player *player)
{
	if (player->livesCount > -1) { player->livesCount--; }
}

void useItem(Player *player)
{
	switch (player->currentItem)
	{
		case CHEVERE_HAT:
			player->chevereMode = true;
			break;
		default: break;
	}
	player->currentItem = NONE;
}

void renderPlayer(Player *player)
{
	playMovingObjectAnimation(player->avatar.anim, 7);
	if (player->chevereMode)
		playSpriteAnimation(&player->avatar.animations.hat, 7);
}