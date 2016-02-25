#include "Title.hpp"

#include <cstdio>
#include <cmath>

const std::string Title::sGreetStr = std::string("Hi ! Special thanks to xerpi, akabane87, al3x_10m, ezi0, netrix, phaazon, maracuja, Adoru, p0nce, flure, wullon, zerkman, nanard, sam, Msk, " \
	"latortue, g012, RaHoW, cyberpingui, rez, dubmood, yogibear, kaneel, Zorro 2, Den, titeiko, and well I may forgot people as you are many that I love being with you on IRC ! " \
	"Oh! There also the great people in #openpandora: ptitSeb, sebt3, Cloudef, Elw3, endrift, klopsi-u3, Lolla, TrashyMG, undexsym, urjaman, and Wally ^^ Thanks to rejuvenate team :) All graphics used here comes from Google Material icons, font is Droid Sans. " \
	" This program use the color scheme you can found in flood-it for Kolibrios, by Leency.        See you later :)");

Title::Title() : GameState() {
}

Title::~Title() {
}

bool Title::enter() {
	mGraphicsBase = GraphicsBase::instance();
	mGraphics2D = Graphics2D::instance();
	mGameContext = GameContext::instance();
	mDirector = Director::instance();
	mSpriteBuffer = mGameContext->getSpriteBuffer();

	mPlaySprite = Sprite();
	mPlaySprite.frame = mGameContext->getSpritesCatalog()->getFrame("play");
	mPlaySprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
	mPlaySprite.position.y = GraphicsBase::DISPLAY_HEIGHT*0.65f;
	mPlaySprite.color.a = 255;

	mTitleSprite = Sprite();
	mTitleSprite.frame = mGameContext->getSpritesCatalog()->getFrame("title");
	mTitleSprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
	mTitleSprite.position.y = GraphicsBase::DISPLAY_HEIGHT*0.2f;
	mTitleSprite.color.a = 255;

	mOverlaySprite = Sprite();
	mOverlaySprite.color.r = 0;
	mOverlaySprite.color.g = 0;
	mOverlaySprite.color.b = 0;
	mOverlaySprite.color.a = 255;
	mOverlaySprite.frame.coords = mGameContext->getSpritesCatalog()->getFrame("tile").coords;
	mOverlaySprite.frame.size.w = GraphicsBase::DISPLAY_WIDTH;
	mOverlaySprite.frame.size.h = GraphicsBase::DISPLAY_HEIGHT;
	mOverlaySprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
	mOverlaySprite.position.y = GraphicsBase::DISPLAY_HEIGHT/2;

	mScrollSprite = SpriteLetter();
	mScrollSprite.frame.size.w = Title::CHAR_SIZE;
	mScrollSprite.frame.size.h = Title::CHAR_SIZE;
	mScrollSprite.color.r = 0;
	mScrollSprite.color.g = 0;
	mScrollSprite.color.b = 0;
	mScrollOffset = (float) (GraphicsBase::DISPLAY_WIDTH + Title::CHAR_SIZE);
	mScrollTreshold = -(Title::sGreetStr.length() * (float) (Title::CHAR_SIZE-Title::CHAR_OFFSET));

	mStep = ENTER;
	return true;
}

void Title::exit() {
}

void Title::update(const SceCtrlData& inputs, const SceTouchData& touchFront, const SceTouchData& touchBack, float elapsed) {
	Sprite* floorSprite = mGameContext->getFloorSprite();
	glm::mat4* ortho = mGameContext->getOrthogonalMatrix();

	// Update menu
	if (mStep == IDLE) {
		if (touchFront.reportNum > 0) {
			mTouchX = touchFront.report[0].x*0.5f;
			mTouchY = touchFront.report[0].y*0.5f;

			if (Utils::isTouched(mTouchX, mTouchY, mPlaySprite)) {
				// Play button is touchover
				mPlaySprite.color.r = mPlaySprite.color.g = mPlaySprite.color.b = 128;
				mTouchedFlag = true;
			} else {
				mPlaySprite.color.r = mPlaySprite.color.g = mPlaySprite.color.b = 255;
			}
		} else {
			if (mTouchedFlag) {
				mTouchedFlag = false;
				if (Utils::isTouched(mTouchX, mTouchY, mPlaySprite)) {
					mPlaySprite.color.r = mPlaySprite.color.g = mPlaySprite.color.b = 255;
					mStep = EXIT_TO_GAME;
				}
			}
		}
	}

	// Update scene
	float val = elapsed * 600;
	switch(mStep) {
		case ENTER:
			mOverlaySprite.color.a-= val;
			if (mOverlaySprite.color.a <= 6) {
				mStep = IDLE;
				mOverlaySprite.color.a = 0;
			}
		break;
		case EXIT_TO_GAME:
			mTitleSprite.color.a-= val;
			mPlaySprite.color.a-= val;
			if (mTitleSprite.color.a <= 6) {
				mStep = STOP;
				mDirector->changeState("Game");
			}
		break;
		case IDLE:
		case STOP: break;
	}

	// Floor texture scroll
	float mov = elapsed*0.05f;
	float mov1 = mov/2;
	floorSprite->frame.coords.s += mov;
	floorSprite->frame.coords.t += mov1;
	floorSprite->frame.coords.u += mov;
	floorSprite->frame.coords.v += mov1;

	mScrollOffset -= elapsed*250;
	if (mScrollOffset <= mScrollTreshold) {
		mScrollOffset = (float) (GraphicsBase::DISPLAY_WIDTH + Title::CHAR_SIZE);
	}

	float scale = 1.2f + std::cos((floorSprite->frame.coords.s*1500) * M_PI / 180.0f) * 0.1f;
	scale += std::sin((floorSprite->frame.coords.v*2000) * M_PI / 180.0f) * 0.05f;
	mTitleSprite.scale.w = mTitleSprite.scale.h = scale;

	// Render
	mGraphicsBase->startDrawing();
		mGraphics2D->use();
		mGraphics2D->setProjectionMatrix(*ortho);

		mSpriteBuffer->start();
		mSpriteBuffer->put(*floorSprite);
		mGraphics2D->setTexture(mGameContext->getFloorTexture());
		mGraphics2D->render(mSpriteBuffer);

		mSpriteBuffer->put(mTitleSprite);
		mSpriteBuffer->put(mPlaySprite);
		mGraphics2D->setTexture(mGameContext->getSpritesTexture());
		mGraphics2D->render(mSpriteBuffer);

		mSpriteBuffer->put(mScrollOffset, GraphicsBase::DISPLAY_HEIGHT-Title::CHAR_SIZE/2, -Title::CHAR_OFFSET, mScrollSprite, sGreetStr);
		mGraphics2D->setTexture(mGameContext->getFontTexture());
		mGraphics2D->render(mSpriteBuffer);

		if (mOverlaySprite.color.a > 0) {
			mSpriteBuffer->put(mOverlaySprite);
			mGraphics2D->setTexture(mGameContext->getSpritesTexture());
			mGraphics2D->render(mSpriteBuffer);
		}

#ifdef __DEBUG__
		char str[100];
		snprintf(str, 100, "FPS: %i", mDirector->getFPS());
		mSpriteBuffer->put(16,16, 0, *mGameContext->getSpriteLetter(), std::string(str));
		mGraphics2D->setTexture(GraphicsBase::getDebugFontTexture());
		mGraphics2D->render(mSpriteBuffer);
#endif
	mGraphicsBase->stopDrawing();
	mGraphicsBase->swapBuffers();
}

const std::string Title::getName() {
	return "Title";
}
