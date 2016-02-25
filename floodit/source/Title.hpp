#pragma once
#include <string>
#include <psp2/ctrl.h>
#include <glm/glm.hpp>
#include <notengine/notengine.hpp>

#include "GameContext.hpp"

using namespace NotEngine::Graphics;
using namespace NotEngine::Game;

class Title : public GameState {

	private:
		Title(const Title& copy);
		void operator=(Title const&);

		SpriteLetter mScrollSprite;
		Sprite mOverlaySprite;
		Sprite mPlaySprite;
		Sprite mTitleSprite;
		float mScrollOffset;
		float mScrollTreshold;

		bool mTouchedFlag;
		int mTouchX;
		int mTouchY;

		SpriteBuffer* mSpriteBuffer;
		GraphicsBase* mGraphicsBase;
		Graphics2D* mGraphics2D;
		GameContext* mGameContext;
		Director* mDirector;

		enum Step {
			ENTER,
			EXIT_TO_GAME,
			IDLE,
			STOP
		};
		Step mStep;

		static const unsigned int CHAR_SIZE = 48;
		static const int CHAR_OFFSET = 16;
		static const std::string sGreetStr;

	public:
		Title();
		virtual ~Title();

		virtual bool enter ();
		virtual void exit ();

		virtual void update (const SceCtrlData& inputs, const SceTouchData& touchFront, const SceTouchData& touchBack, float elapsed);
		virtual const std::string getName();

};
