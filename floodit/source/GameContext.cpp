#include "GameContext.hpp"

#include <cstring>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <psp2shell.h>

#include "../data/floortex.raw.h"
#include "../data/spritesheet.raw.h"
#include "../data/gamefont.raw.h"
#include "../data/spritesheet.h"

extern bool psp2shell;

GameContext::GameContext() {
}

GameContext::~GameContext() {
}

int GameContext::initialize() {
	int ret;

	mSpriteBuffer = 0;
	mSpriteBuffer = new SpriteBuffer();
	ret = mSpriteBuffer->initialize(Graphics2D::MAX_SPRITES_PER_BATCH, true);
	if (ret != SpriteBuffer::NO_ERROR) {
#ifdef DEBUG
		if (psp2shell) psp2shell_print("spriteBuffer->initialize() fail: %d\n", ret);
#endif
		return SPRITE_BUFFER;
	}

	mSpritesCatalog = 0;
	mSpritesCatalog = new FrameCatalog();
	ret = mSpritesCatalog->initialize(std::string(&spritesheet[0]));
	if (ret != FrameCatalog::NO_ERROR) {
#ifdef DEBUG
		if (psp2shell) psp2shell_print("spriteCatalog->initialize() fail: %d\n", ret);
#endif
		return SPRITE_CATALOG;
	}

	mSpritesTexture = 0;
	mSpritesTexture = new Texture2D();
	ret = mSpritesTexture->initialize(mSpritesCatalog->getWidth(), mSpritesCatalog->getHeight(), SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	if (ret != Texture2D::NO_ERROR) {
#ifdef DEBUG
		if (psp2shell) psp2shell_print("spritesTexture->initialize() fail: %d\n", ret);
#endif
		return SPRITES_TEXTURE;
	}
	unsigned int stride = Texture2D::getStride(mSpritesCatalog->getWidth(), SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	unsigned int raw = mSpritesCatalog->getWidth() * Texture2D::getStorageSize(SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	char* texPtr = (char*) mSpritesTexture->getDataPtr();
	for (unsigned int y=0; y<mSpritesCatalog->getHeight(); y++) {
		memcpy(&texPtr[y*stride], &SPRITESHEET_RAW[y*raw], raw);
	}
	mSpritesTexture->setFilter(SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);

	mFloorTexture = 0;
	mFloorTexture = new Texture2D();
	ret = mFloorTexture->initialize(16, 16, SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	if (ret != Texture2D::NO_ERROR) {
#ifdef DEBUG
		if (psp2shell) psp2shell_print("floorTexture->initialize() fail: %d\n", ret);
#endif
		return FLOOR_TEXTURE;
	}
	stride = Texture2D::getStride(16, SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	raw = 16 * Texture2D::getStorageSize(SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	texPtr = (char*) mFloorTexture->getDataPtr();
	for (unsigned int y=0; y<16; y++) {
		memcpy(&texPtr[y*stride], &FLOORTEX_RAW[y*raw], raw);
	}
	mFloorTexture->setWrap(SCE_GXM_TEXTURE_ADDR_REPEAT, SCE_GXM_TEXTURE_ADDR_REPEAT);

	mFontTexture = 0;
	mFontTexture = new Texture2D();
	ret = mFontTexture->initialize(512, 512, SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	if (ret != Texture2D::NO_ERROR) {
#ifdef DEBUG
		if (psp2shell) psp2shell_print("fontTexture->initialize() fail: %d\n", ret);
#endif
		return FONT_TEXTURE;
	}
	mFontTexture->setFilter(SCE_GXM_TEXTURE_FILTER_LINEAR, SCE_GXM_TEXTURE_FILTER_LINEAR);

	stride = Texture2D::getStride(512, SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	raw = 512 * Texture2D::getStorageSize(SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR);
	texPtr = (char*) mFontTexture->getDataPtr();
	for (unsigned int y=0; y<512; y++) {
		memcpy(&texPtr[y*stride], &GAMEFONT_RAW[y*raw], raw);
	}

	mOrtho = glm::ortho(
		0.0f, (float) GraphicsBase::DISPLAY_WIDTH,
		(float) GraphicsBase::DISPLAY_HEIGHT, 0.0f,
		-1.0f, 1.0f);

	// Debug sprite letter
	mSpriteLetter = SpriteLetter();
	mSpriteLetter.frame.size.w = 16;
	mSpriteLetter.frame.size.h = 16;
	mSpriteLetter.color.a = 255;
	mSpriteLetter.color.r = mSpriteLetter.color.g = mSpriteLetter.color.b = 0;

	// Scrolling texture floor
	mFloorSprite = Sprite();
	mFloorSprite.frame.coords.u = 2;
	mFloorSprite.frame.coords.v = 2;
	mFloorSprite.frame.size.w = GraphicsBase::DISPLAY_WIDTH;
	mFloorSprite.frame.size.h = GraphicsBase::DISPLAY_WIDTH;
	mFloorSprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
	mFloorSprite.position.y = GraphicsBase::DISPLAY_HEIGHT/2;

	return NO_ERROR;
}

void GameContext::finalize() {
	if (mSpriteBuffer != 0) {
		mSpriteBuffer->finalize();
		delete mSpriteBuffer;
	}

	if (mSpritesCatalog != 0) {
		mSpritesCatalog->finalize();
		delete mSpritesCatalog;
	}

	if(mSpritesTexture != 0) {
		mSpritesTexture->finalize();
		delete mSpritesTexture;
	}

	if(mFloorTexture != 0) {
		mFloorTexture->finalize();
		delete mFloorTexture;
	}

	if (mFontTexture != 0) {
		mFontTexture->finalize();
		delete mFontTexture;
	}
}

Texture2D* GameContext::getFloorTexture() {
	return mFloorTexture;
}

Texture2D* GameContext::getSpritesTexture() {
	return mSpritesTexture;
}

Texture2D* GameContext::getFontTexture() {
	return mFontTexture;
}

FrameCatalog* GameContext::getSpritesCatalog() {
	return mSpritesCatalog;
}

glm::mat4* GameContext::getOrthogonalMatrix() {
	return &mOrtho;
}

Sprite* GameContext::getFloorSprite() {
	return &mFloorSprite;
}

SpriteLetter* GameContext::getSpriteLetter() {
	return &mSpriteLetter;
}

SpriteBuffer* GameContext::getSpriteBuffer() {
	return mSpriteBuffer;
}
