#ifndef GUI_H
#define GUI_H

#include "animation.h"
#include "player.h"

// Bottom Screen animations for...
// ... the layout (images)
extern ImageAnimationFrame layoutFrames[1];
extern ImageAnimation layoutAnimation;
// ... the button (images)
extern ImageAnimationFrame buttonFrames[1];
extern ImageAnimation buttonAnimation;
extern ImageAnimationFrame buttonPressedFrames[1];
extern ImageAnimation buttonPressedAnimation;
// ... the life/death icons (sprites)
extern SpriteAnimationFrame lifeIconsFrames[3][1];
extern SpriteAnimation lifeIconsAnimations[3];
extern SpriteAnimationFrame deathIconsFrames[3][1];
extern SpriteAnimation deathIconsAnimations[3];
// ... the flies icons (sprites)
extern SpriteAnimationFrame flyIconsFrames[3][2];
extern SpriteAnimation flyIconsAnimations[3];
// ... the water barrel (sprites)
extern SpriteAnimationFrame barrelFrontFrames[1];
extern SpriteAnimation barrelFrontAnimation;
extern SpriteAnimationFrame barrelContentFrames[101];
extern SpriteAnimation barrelContentAnimation;
extern SpriteAnimationFrame barrelBackFrames[1];
extern SpriteAnimation barrelBackAnimation;
// ... the phase numbers (sprites)
extern SpriteAnimationFrame phaseNumbersSprites[9][20];
extern SpriteAnimation phaseNumbersAnimations[10];
// ... the items (sprites)
extern SpriteAnimationFrame itemsFrames[2][20];
extern SpriteAnimation itemsAnimations[2];

void initGUI();

void displayGUI(Player *player);

#endif