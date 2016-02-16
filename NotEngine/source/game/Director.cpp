#include "../../include/notengine/game/Director.hpp"

#include <cstdio>
#include <psp2/rtc.h>

namespace NotEngine {

	namespace Game {

		Director::Director() : System::Singleton<Director>() {
			printf("Director()\n");
		}

		Director::~Director() {
			printf("~Director()\n");
		}

		bool Director::initialize(std::map<std::string, GameState*> states, std::string start) {
			mGameStates = states;

			if (mGameStates.find(start) == mGameStates.end()) {
				printf("Director cannot find first game state: %s\n", start.c_str());
				return false;
			}

			mPendingState = 0;
			mCurrentState = states[start];
			if(! mCurrentState->enter()) {
				printf("Director GameState failed to initialize\n");
				return false;
			}

			sceRtcGetCurrentTick(&mCurrentTicks);
			mElapsed = 0;
			mFrames = 0;
			mLastTicks = mCurrentTicks;
			mLastFpsTicks = mCurrentTicks;
			return true;
		}

		void Director::finalize() {
		}

		void Director::update() {
			const float tickRate = 1.0f / sceRtcGetTickResolution();

			sceRtcGetCurrentTick(&mCurrentTicks);
			mElapsed = (float) (mCurrentTicks - mLastTicks) * tickRate;
			mLastTicks = mCurrentTicks;

			if((mCurrentTicks - mLastFpsTicks) * tickRate >= 1) {
				mLastFpsTicks = mCurrentTicks;
				mFps = mFrames;
				mFrames = 0;
			}
			mFrames++;

			sceTouchPeek(SCE_TOUCH_PORT_FRONT, &mTouchFrontData, 1);
			sceTouchPeek(SCE_TOUCH_PORT_BACK, &mTouchBackData, 1);
			sceCtrlPeekBufferPositive(0, &mPadData, 1);

			if (mPendingState != 0) {
				mCurrentState->exit();
				mCurrentState = mPendingState;
				mPendingState = 0;

				if (! mCurrentState->enter()) {
					printf("Director: GameState failed to initialize\n");
				}
			}
			mCurrentState->update(&mPadData, &mTouchFrontData, &mTouchBackData, mElapsed);
		}

		bool Director::isRunning() {
			return !mQuit;
		}

		void Director::stop() {
			mCurrentState->exit();
			mQuit = true;
		}

		bool Director::changeState(const std::string name) {
			if (mGameStates.find(name) != mGameStates.end()) {
				mPendingState = mGameStates[name];
				return true;
			} else {
				return false;
			}
		}

		unsigned int Director::getFPS() {
			return mFps;
		}

	} // namespace Game

} // namespace NotEngine
