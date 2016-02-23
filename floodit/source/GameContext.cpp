#include "GameContext.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "../data/floortex.raw.h"
#include "../data/spritesheet.raw.h"
#include "../data/gamefont.raw.h"
#include "../data/spritesheet.h"

GameContext::GameContext() {
}

GameContext::~GameContext() {
}

bool GameContext::initialize() {
	mSpriteBuffer = 0;
	mSpriteBuffer = new SpriteBuffer();
	if (!mSpriteBuffer->initialize(1024, true)) {
		printf("SpriteBuffer failed\n");
		return false;
	}


	mSpritesCatalog = 0;
	mSpritesCatalog = new FrameCatalog();
	if (!mSpritesCatalog->initialize(std::string(&spritesheet[0]))) {
		printf("mSpriteCatalog failed\n");
		return false;
	}

	mSpritesTexture = 0;
	mSpritesTexture = new Texture2D();
	if (!mSpritesTexture->initialize(mSpritesCatalog->getWidth(), mSpritesCatalog->getHeight(), SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR)) {
		printf("mSpritesTexture failed\n");
		return false;
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
	if (!mFloorTexture->initialize(16, 16, SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR)) {
		printf("mFloorTexture failed\n");
		return false;
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
	if (!mFontTexture->initialize(512, 512, SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR)) {
		printf("mFontTexture failed\n");
		return false;
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

	return true;
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
