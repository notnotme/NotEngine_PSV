#include "Game.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

Game::Game() : GameState() {
}

Game::~Game() {
}

int Game::enter() {
	mGraphicsBase = GraphicsBase::instance();
	mGraphics2D = Graphics2D::instance();
	mGameContext = GameContext::instance();
	mDirector = Director::instance();
	mSpriteBuffer = mGameContext->getSpriteBuffer();

	// Precompute some values to ease the drawing
	int playerTileSize = Game::PLAYER_GRID_TILE_SIZE + Game::PLAYER_GRID_TILE_MARGIN;
	int gameTileSize = Game::GRID_TILE_SIZE + Game::GRID_TILE_MARGIN;
	int gameGridSize = gameTileSize * Game::GRID_SIZE;
	int playerGridSize = playerTileSize * Game::PLAYER_GRID_H;

	mGameGridMarginX = GraphicsBase::DISPLAY_WIDTH - gameGridSize - gameTileSize/2;
	mGameGridMarginY = (GraphicsBase::DISPLAY_HEIGHT - gameGridSize)/2 + gameTileSize/2;

	mPlayerGridMarginX = Game::PLAYER_GRID_TILE_SIZE/2 + Game::GRID_TILE_SIZE;
	mPlayerGridMarginY = GraphicsBase::DISPLAY_HEIGHT - mGameGridMarginY - playerGridSize
		+ playerTileSize/2 + Game::PLAYER_GRID_TILE_MARGIN*2 + Game::GRID_TILE_MARGIN*2;

	mScoreMarginX = mPlayerGridMarginX - playerTileSize/2 + Game::CHAR_SIZE/2 + 8; // (+8 to center it..)
	mScoreMarginY = mPlayerGridMarginY - playerTileSize/2 - Game::CHAR_SIZE;

	mLoseMarginX = GraphicsBase::DISPLAY_WIDTH/2 - (strlen("you LOSE :(") * (Game::CHAR_SIZE-Game::CHAR_OFFSET))/2 + Game::CHAR_OFFSET;
	mWinMarginX = GraphicsBase::DISPLAY_WIDTH/2 - (strlen("you WIN :(") * (Game::CHAR_SIZE-Game::CHAR_OFFSET))/2 + Game::CHAR_OFFSET;

	// Initialize our sprites
	FrameCatalog::Frame tileFrame = mGameContext->getSpritesCatalog()->getFrame("tile");
	for (unsigned int y=0; y<Game::GRID_SIZE; y++) {
		for(unsigned int x=0; x<Game::GRID_SIZE; x++) {
			GameSprite gameSprite = GameSprite();
			gameSprite.frame.coords = tileFrame.coords;
			gameSprite.frame.size.h = Game::GRID_TILE_SIZE;
			gameSprite.frame.size.w = Game::GRID_TILE_SIZE;

			gameSprite.position.x = mGameGridMarginX + (x* (Game::GRID_TILE_SIZE+Game::GRID_TILE_MARGIN));
			gameSprite.position.y = mGameGridMarginY + (y* (Game::GRID_TILE_SIZE+Game::GRID_TILE_MARGIN));

			int offset = (y*Game::GRID_SIZE)+x;
			mGameGrid[offset] = gameSprite;
			mNewGameGrid[offset] = gameSprite;
		}
	}

	for (unsigned int y=0; y<Game::PLAYER_GRID_H; y++) {
		for (unsigned int x=0; x<Game::PLAYER_GRID_W; x++) {
			GameSprite gameSprite = GameSprite();
			gameSprite.frame.coords = tileFrame.coords;
			gameSprite.frame.size.h = Game::PLAYER_GRID_TILE_SIZE;
			gameSprite.frame.size.w = Game::PLAYER_GRID_TILE_SIZE;

			gameSprite.position.x = mPlayerGridMarginX + (x* (Game::PLAYER_GRID_TILE_SIZE+Game::PLAYER_GRID_TILE_MARGIN));
			gameSprite.position.y = mPlayerGridMarginY + (y* (Game::PLAYER_GRID_TILE_SIZE+Game::PLAYER_GRID_TILE_MARGIN));
			mPlayerGrid[(y*Game::PLAYER_GRID_W)+x] = gameSprite;
		}
	}

	mTopOverlaySprite = Sprite();
	mTopOverlaySprite.color.r = mTopOverlaySprite.color.g = mTopOverlaySprite.color.b = 255;
	mTopOverlaySprite.color.a = 0;
	mTopOverlaySprite.frame.coords = mGameContext->getSpritesCatalog()->getFrame("tile").coords;
	mTopOverlaySprite.frame.size.w = GraphicsBase::DISPLAY_WIDTH;
	mTopOverlaySprite.frame.size.h = GraphicsBase::DISPLAY_HEIGHT;
	mTopOverlaySprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
	mTopOverlaySprite.position.y = GraphicsBase::DISPLAY_HEIGHT/2;

	mBackOverlaySprite = Sprite();
	mBackOverlaySprite.color.a = 0;
	mBackOverlaySprite.frame.coords = mGameContext->getSpritesCatalog()->getFrame("tile").coords;
	mBackOverlaySprite.frame.size.w = GraphicsBase::DISPLAY_WIDTH;
	mBackOverlaySprite.frame.size.h = GraphicsBase::DISPLAY_HEIGHT;
	mBackOverlaySprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
	mBackOverlaySprite.position.y = GraphicsBase::DISPLAY_HEIGHT/2;

	mRestartSprite = Sprite();
	mRestartSprite.color.a = 0;
	mRestartSprite.frame = mGameContext->getSpritesCatalog()->getFrame("restart");
	mRestartSprite.position.x = GraphicsBase::DISPLAY_WIDTH/2 + mRestartSprite.frame.size.w;
	mRestartSprite.position.y = GraphicsBase::DISPLAY_HEIGHT*0.70f;
	mRestartSprite.color.r = mRestartSprite.color.b = 128;
	mRestartSprite.color.g = 255;

	mQuitSprite = Sprite();
	mQuitSprite.color.a = 0;
	mQuitSprite.frame = mGameContext->getSpritesCatalog()->getFrame("stop");
	mQuitSprite.position.x = GraphicsBase::DISPLAY_WIDTH/2 - mQuitSprite.frame.size.w;
	mQuitSprite.position.y = GraphicsBase::DISPLAY_HEIGHT*0.70f;
	mQuitSprite.color.g = mQuitSprite.color.b = 128;
	mQuitSprite.color.r = 255;

	mGameOverSpriteLetter = SpriteLetter();
	mGameOverSpriteLetter.frame.size.w = Game::CHAR_SIZE;
	mGameOverSpriteLetter.frame.size.h = Game::CHAR_SIZE;
	mGameOverSpriteLetter.color.r = 0;
	mGameOverSpriteLetter.color.g = 0;
	mGameOverSpriteLetter.color.b = 0;

	mScoreSpriteLetter = SpriteLetter();
	mScoreSpriteLetter.frame.size.w = Game::CHAR_SIZE;
	mScoreSpriteLetter.frame.size.h = Game::CHAR_SIZE;
	mScoreSpriteLetter.color.r = 0;
	mScoreSpriteLetter.color.g = 0;
	mScoreSpriteLetter.color.b = 0;

	newGame();
	return NO_ERROR;
}

void Game::exit() {
}

void Game::updateControls(const SceTouchData& touchFront) {
	// Update controls if touched and idle state
	if (mStep == IDLE || mStep == GAMEOVER_IDLE) {
		if (touchFront.reportNum > 0) {
			mTouchX = touchFront.report[0].x*0.5f;
			mTouchY = touchFront.report[0].y*0.5f;

			if (mStep == IDLE) {
				for(unsigned int i=0; i<Game::GRID_COLORS; i++) {
					if (Utils::isTouched(mTouchX, mTouchY, mPlayerGrid[i])) {
						mPlayerGrid[i].color.a = 75;
						mTouchedFlag = true;
					} else {
						mPlayerGrid[i].color.a = 255;
					}
				}
			} else {
				if (Utils::isTouched(mTouchX, mTouchY, mQuitSprite)) {
					mQuitSprite.color.g = mQuitSprite.color.b = 196;
					mQuitSprite.color.r = 255;
					mTouchedFlag = true;
				} else {
					mQuitSprite.color.g = mQuitSprite.color.b = 128;
					mQuitSprite.color.r = 255;
				}

				if (Utils::isTouched(mTouchX, mTouchY, mRestartSprite)) {
					mRestartSprite.color.r = mRestartSprite.color.b = 196;
					mRestartSprite.color.g = 255;
					mTouchedFlag = true;
				} else {
					mRestartSprite.color.r = mRestartSprite.color.b = 128;
					mRestartSprite.color.g = 255;
				}
			}
		} else {
			if (mTouchedFlag) {
				mTouchedFlag = false;

				if (mStep == IDLE) {
					for(unsigned int i=0; i<Game::GRID_COLORS; i++) {
						if (Utils::isTouched(mTouchX, mTouchY, mPlayerGrid[i])) {
							mPlayerGrid[i].color.a = 255;
							// Play button was clicked
							if (mPlayerGrid[i].getIntColor() != mGameGrid[0].getIntColor()) {
								mState = playTurn(mPlayerGrid[i].getIntColor());
								mStep = TURN_START;
							}
						}
					}
				} else {
					if (Utils::isTouched(mTouchX, mTouchY, mQuitSprite)) {
						mQuitSprite.frame = mGameContext->getSpritesCatalog()->getFrame("stop");
						mQuitSprite.color.g = mQuitSprite.color.b = 128;
						mQuitSprite.color.r = 255;
						mStep = GAMEOVER_END;
					}
					if (Utils::isTouched(mTouchX, mTouchY, mRestartSprite)) {
						mRestartSprite.color.r = mRestartSprite.color.b = 128;
						mRestartSprite.color.g = 255;
						mStep = GAMEOVER_END;
						mState = CONTINUE;
					}
				}
			}
		}
	}
}

int Game::updateScene(float elapsed) {
	// Update scene and logic according to mStep
	switch (mStep) {
		case ENTER:
			if (mGameGrid[0].color.a < 220) {
				float val = elapsed * 600;
				if (mBackOverlaySprite.color.a < 220) {
					mBackOverlaySprite.color.a += val;
				}
				mScoreSpriteLetter.color.a += val;
				for(unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
					if (i < Game::GRID_COLORS) mPlayerGrid[i].color.a += val;
					mGameGrid[i].color.a += val;
				}
			} else {
				mStep = IDLE;
				mBackOverlaySprite.color.a = 220;
				mScoreSpriteLetter.color.a = 255;
				for(unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
					if (i < Game::GRID_COLORS) 	mPlayerGrid[i].color.a = 255;
					mGameGrid[i].color.a = 255;
				}
			}
		break;
		case EXIT:
			if (mTopOverlaySprite.color.a < 235) {
				float val = elapsed * 600;
				mTopOverlaySprite.color.a += val;
			} else {
				mStep = STOP;
				int ret = mDirector->changeState("Title");
				if (ret != Director::NO_ERROR) {
					return ret;
				}
			}
		break;
		case TURN_START:
			if (mGameGrid[0].color.a > 10) {
				float val = elapsed * 1200;
				for(unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
					if(mGameGrid[i].getIntColor() != mNewGameGrid[i].getIntColor()) {
						mGameGrid[i].color.a -= val;
					}
				}
			} else {
				mStep = TURN_END;
				for(unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
					if(mGameGrid[i].getIntColor() != mNewGameGrid[i].getIntColor()) {
						mGameGrid[i].setIntColor(
							mNewGameGrid[i].getIntColor()
						);
						mGameGrid[i].color.a = 0;
					}
				}
			}
		break;
		case TURN_END:
			if (mGameGrid[0].color.a < 235) {
				float val = elapsed * 1200;
				for(unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
					if (mGameGrid[i].color.a < 235) {
						mGameGrid[i].color.a += val;
					}
				}
			} else {
				for(unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
					mGameGrid[i].color.a = 255;
				}

				switch(mState) {
					case WIN:
					case LOSE: mStep = GAMEOVER_START; break;
					case CONTINUE: mStep = IDLE; break;
				}
			}
		break;
		case GAMEOVER_START:
			if (mTopOverlaySprite.color.a < 220) {
				float val = elapsed * 600;
				mTopOverlaySprite.color.a += val;
				mRestartSprite.color.a += val;
				mQuitSprite.color.a += val;
				mGameOverSpriteLetter.color.a += val;
			} else {
				mStep = GAMEOVER_IDLE;
				mRestartSprite.color.a = 255;
				mQuitSprite.color.a = 255;
				mGameOverSpriteLetter.color.a = 255;
			}
		break;
		case GAMEOVER_END:
			if (mTopOverlaySprite.color.a > 10) {
				float val = elapsed * 600;
				mTopOverlaySprite.color.a -= val;
				mRestartSprite.color.a -= val;
				mQuitSprite.color.a -= val;
				mGameOverSpriteLetter.color.a -= val;
				if (mState == CONTINUE) {
					mScoreSpriteLetter.color.a -= val;
					for(unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
						if (i < Game::GRID_COLORS) mPlayerGrid[i].color.a -= val;
						mGameGrid[i].color.a -= val;
					}
				}
			} else {
				mTopOverlaySprite.color.a = 0;
				mRestartSprite.color.a = 0;
				mQuitSprite.color.a = 0;
				mGameOverSpriteLetter.color.a = 0;
				if (mState == WIN || mState == LOSE) {
					mTopOverlaySprite.color. r = mTopOverlaySprite.color. g = mTopOverlaySprite.color. b = 0;
					mStep = EXIT;
				} else {
					newGame();
				}
			}
		break;

		case GAMEOVER_IDLE:
		case IDLE:
		case STOP: break;
	}

	// Update floor texture scroll
	Sprite* floorSprite = mGameContext->getFloorSprite();
	float mov = elapsed*0.05f;
	float mov1 = mov/2;
	floorSprite->frame.coords.s += mov;
	floorSprite->frame.coords.t += mov1;
	floorSprite->frame.coords.u += mov;
	floorSprite->frame.coords.v += mov1;
	return NO_ERROR;
}

int Game::update(const SceCtrlData& inputs, const SceTouchData& touchFront, const SceTouchData& touchBack, float elapsed) {
	glm::mat4* ortho = mGameContext->getOrthogonalMatrix();
	Sprite* floorSprite = mGameContext->getFloorSprite();

	updateControls(touchFront);
	int ret = updateScene(elapsed);
	if (ret != NO_ERROR) {
		return ret;
	}

	mGraphicsBase->startDrawing();
		mGraphics2D->use();
		mGraphics2D->setProjectionMatrix(*ortho);
		mSpriteBuffer->start();

		mSpriteBuffer->put(*floorSprite);
		mGraphics2D->setTexture(mGameContext->getFloorTexture());
		mGraphics2D->render(mSpriteBuffer);

		mSpriteBuffer->put(mBackOverlaySprite);
		for(unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
			mSpriteBuffer->put(mGameGrid[i]);
			if (i < Game::GRID_COLORS)
				mSpriteBuffer->put(mPlayerGrid[i]);
		}
		mGraphics2D->setTexture(mGameContext->getSpritesTexture());
		mGraphics2D->render(mSpriteBuffer);

		char str[25];
		snprintf(str, 25, "Left: %2i", mClicks);
		mSpriteBuffer->put(mScoreMarginX,mScoreMarginY, -16, mScoreSpriteLetter, std::string(str));
		mGraphics2D->setTexture(mGameContext->getFontTexture());
		mGraphics2D->render(mSpriteBuffer);

		if (mTopOverlaySprite.color.a != 0) {
			mSpriteBuffer->put(mTopOverlaySprite);
			mSpriteBuffer->put(mQuitSprite);
			mSpriteBuffer->put(mRestartSprite);
			mGraphics2D->setTexture(mGameContext->getSpritesTexture());
			mGraphics2D->render(mSpriteBuffer);

			if (mGameOverSpriteLetter.color.a != 0) {
				if (mState == WIN) {
					mSpriteBuffer->put(mWinMarginX,GraphicsBase::DISPLAY_HEIGHT*0.4, -Game::CHAR_OFFSET, mGameOverSpriteLetter, "You WIN :)");
					mGraphics2D->setTexture(mGameContext->getFontTexture());
					mGraphics2D->render(mSpriteBuffer);
				} else if (mState == LOSE) {
					mSpriteBuffer->put(mLoseMarginX,GraphicsBase::DISPLAY_HEIGHT*0.4, -Game::CHAR_OFFSET, mGameOverSpriteLetter, "You LOSE :(");
					mGraphics2D->setTexture(mGameContext->getFontTexture());
					mGraphics2D->render(mSpriteBuffer);
				}
			}
		}

#ifdef DEBUG
		snprintf(str, 25, "FPS: %i", mDirector->getFPS());
		mSpriteBuffer->put(16,16, -4, *mGameContext->getSpriteLetter(), std::string(str));
		mGraphics2D->setTexture(GraphicsBase::getDebugFontTexture());
		mGraphics2D->render(mSpriteBuffer);
#endif
	mGraphicsBase->stopDrawing();
	mGraphicsBase->swapBuffers();
	return NO_ERROR;
}

const std::string Game::getName() {
	return "Game";
}

void Game::newGame() {
	mStep = ENTER;
	mState = CONTINUE;
	mClicks = Game::MAX_CLICKS;

	mPlayerGrid[0].setIntColor(Game::COLOR_1);
	mPlayerGrid[1].setIntColor(Game::COLOR_2);
	mPlayerGrid[2].setIntColor(Game::COLOR_3);
	mPlayerGrid[3].setIntColor(Game::COLOR_4);
	mPlayerGrid[4].setIntColor(Game::COLOR_5);
	mPlayerGrid[5].setIntColor(Game::COLOR_6);

	for (unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
		mGameGrid[i].setIntColor(
			mPlayerGrid[rand() % Game::GRID_COLORS].getIntColor()
		);
		mNewGameGrid[i].setIntColor(
			mGameGrid[i].getIntColor()
		);

		mGameGrid[i].color.a = 0;
	}
	for (unsigned int i=0; i<Game::GRID_COLORS; i++) {
		mPlayerGrid[i].color.a = 0;
	}
	mScoreSpriteLetter.color.a = 0;
}

Game::State Game::playTurn(unsigned int color) {
	mClicks--;
	unsigned int oldColor = mNewGameGrid[0].getIntColor();
	mNewGameGrid[0].setIntColor(0);

	// update the new game grid
	bool restart;
	do {
		restart = false;
		for (unsigned int y=0; y<Game::GRID_SIZE; y++) {
			for (unsigned int x=0; x<Game::GRID_SIZE; x++) {
				int offset = (y*Game::GRID_SIZE)+x;
				if (mNewGameGrid[offset].getIntColor() != oldColor) continue;
				if (mNewGameGrid[offset].getIntColor() == 0) continue;

				if (x>0 && mNewGameGrid[offset-1].getIntColor() == 0) mNewGameGrid[offset].setIntColor(0);
				if (y>0 && mNewGameGrid[offset-Game::GRID_SIZE].getIntColor() == 0) mNewGameGrid[offset].setIntColor(0);
				if (x<GRID_SIZE-1 && mNewGameGrid[offset+1].getIntColor() == 0) mNewGameGrid[offset].setIntColor(0);
				if (y<GRID_SIZE-1 && mNewGameGrid[offset+Game::GRID_SIZE].getIntColor() == 0) mNewGameGrid[offset].setIntColor(0);

				if (mNewGameGrid[offset].getIntColor() == 0) restart = true;
			}
		}
	} while(restart);

	bool win = true;
	for (unsigned int i=0; i<Game::GRID_SIZE*Game::GRID_SIZE; i++) {
		if (mNewGameGrid[i].getIntColor() == 0) mNewGameGrid[i].setIntColor(color);
		if (mNewGameGrid[i].getIntColor() != color) win = false;
	}

	if (win) return WIN;
	if (mClicks == 0) return LOSE;
	return CONTINUE;
}
