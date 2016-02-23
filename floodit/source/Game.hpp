#pragma once
#include <string>
#include <psp2/ctrl.h>
#include <notengine/notengine.hpp>

#include "GameContext.hpp"
#include "GameSprite.hpp"

using namespace NotEngine::Graphics;
using namespace NotEngine::Game;

class Game : public GameState {

	private:
	    ////Leency 06.10.2011, Flood-it! v2.41, GPL
		static const unsigned int COLOR_1 = 0xf18db6ff;
		static const unsigned int COLOR_2 = 0x605ca8ff;
		static const unsigned int COLOR_3 = 0xfddc80ff;
		static const unsigned int COLOR_4 = 0xdc4a20ff;
		static const unsigned int COLOR_5 = 0x46b1e2ff;
		static const unsigned int COLOR_6 = 0x7e9d1eff;

		static const unsigned int GRID_SIZE = 14;
		static const unsigned int MAX_CLICKS = 25;
		static const unsigned int GRID_TILE_SIZE = 32;
		static const unsigned int GRID_TILE_MARGIN = 1;

		static const unsigned int GRID_COLORS = 6;
		static const unsigned int PLAYER_GRID_W = 2;
		static const unsigned int PLAYER_GRID_H = 3;
		static const unsigned int PLAYER_GRID_TILE_SIZE = 128;
		static const unsigned int PLAYER_GRID_TILE_MARGIN = 8;

		enum Step {
			ENTER,
			EXIT,
			TURN_START,
			TURN_END,
			IDLE,
			GAMEOVER_START,
			GAMEOVER_END,
			GAMEOVER_IDLE,
			STOP
		};

		enum State {
			WIN,
			LOSE,
			CONTINUE
		};

		Game(const Game& copy);
		void operator=(Game const&);

		SpriteBuffer* mSpriteBuffer;
		GraphicsBase* mGraphicsBase;
		Graphics2D* mGraphics2D;
		GameContext* mGameContext;
		Director* mDirector;

		int mPlayerGridMarginX;
		int mPlayerGridMarginY;
		int mGameGridMarginX;
		int mGameGridMarginY;
		int mScoreMarginX;
		int mScoreMarginY;
		int mLoseMarginX;
		int mWinMarginX;

		GameSprite mPlayerGrid[GRID_COLORS];
		GameSprite mGameGrid[GRID_SIZE*GRID_SIZE];
		GameSprite mNewGameGrid[GRID_SIZE*GRID_SIZE];
		int mClicks;

		SpriteLetter mScoreSpriteLetter;
		SpriteLetter mGameOverSpriteLetter;
		Sprite mBackOverlaySprite;
		Sprite mTopOverlaySprite;
		Sprite mRestartSprite;
		Sprite mQuitSprite;

		bool mTouchedFlag;
		int mTouchX;
		int mTouchY;

		Step mStep;
		State mState;

		void newGame();
		State playTurn(unsigned int color);

		void updateControls(const SceTouchData& touchFront);
		void updateScene(float elapsed);
		void renderScene();

	public:
		Game();
		virtual ~Game();

		virtual bool enter ();
		virtual void exit ();

		virtual void update (const SceCtrlData& inputs, const SceTouchData& touchFront, const SceTouchData& touchBack, float elapsed);
		virtual const std::string getName();

};
