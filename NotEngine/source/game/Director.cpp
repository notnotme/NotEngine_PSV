#include "../../include/notengine/game/Director.hpp"

#include <stdio.h>
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

			mCurrentState = states[start];
			if(! mCurrentState->enter()) {
				printf("Director GameState failed to initialize\n");
				return false;
			}

			return true;
		}

		void Director::finalize() {
		}

		void Director::update() {
			uint64_t currentTicks;
			sceRtcGetCurrentTick(&currentTicks);
		    long elapsed = currentTicks-mLastTicks;
		    mLastTicks = currentTicks;

		    if(currentTicks - mLastFpsTicks >= 1000) {
		        mLastFpsTicks = currentTicks;
		        mFps = mFrames;
		        mFrames = 0;
		    }
		    mFrames++;

			sceCtrlPeekBufferPositive(0, &mPadData, 1);
			mCurrentState->update(&mPadData, elapsed/1000.0f);
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
				mCurrentState->exit();
				mCurrentState = mGameStates[name];
				if (! mCurrentState->enter()) {
					printf("Director: GameState failed to initialize");
					return false;
				}

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
