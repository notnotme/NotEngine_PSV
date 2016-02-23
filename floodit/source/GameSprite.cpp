#include "GameSprite.hpp"

GameSprite::GameSprite() : Sprite() {
}

GameSprite::~GameSprite() {
}

void GameSprite::setIntColor(unsigned int intColor) {
	color.r = (intColor & 0xFF000000) >> 24;
	color.g = (intColor & 0x00FF0000) >> 16;
	color.b = (intColor & 0x0000FF00) >> 8;
	color.a = (intColor & 0x000000FF);
}

unsigned int GameSprite::getIntColor() {
	return ((color.r << 24) | ((color.g)<<16) | ((color.b)<<8) | color.a);
}
