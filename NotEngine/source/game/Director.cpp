#include "../../include/notengine/game/Director.hpp"

#include <psp2/rtc.h>
#include <psp2/display.h>

namespace NotEngine {

	namespace Game {

		Director::Director() : System::Singleton<Director>() {
		}

		Director::~Director() {
		}

		int Director::initialize(std::map<std::string, GameState*> states, const std::string start) {
			mGameStates = states;

			if (mGameStates.find(start) == mGameStates.end()) {
				return STATE_NOT_FOUND;
			}

			mPendingState = 0;
			mCurrentState = states[start];
			if(mCurrentState->enter() != 0) {
				return STATE_INITIALIZE;
			}

			sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
			sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
			sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);

			sceRtcGetCurrentTick(&mCurrentTicks);
			mElapsed = 0;
			mFrames = 0;
			mLastTicks = mCurrentTicks;
			mLastFpsTicks = mCurrentTicks;
			return NO_ERROR;
		}

		void Director::finalize() const {
			sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_STOP);
			sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_STOP);
		}

		int Director::update() {
			const float tickRate = 1.0f / sceRtcGetTickResolution();

			sceRtcGetCurrentTick(&mCurrentTicks);
			mElapsed = (float) (mCurrentTicks.tick - mLastTicks.tick) * tickRate;
			mLastTicks = mCurrentTicks;

			if((mCurrentTicks.tick - mLastFpsTicks.tick) * tickRate >= 1) {
				mLastFpsTicks = mCurrentTicks;
				mFps = mFrames;
				mFrames = 0;
			}
			mFrames++;

			sceTouchPeek(SCE_TOUCH_PORT_FRONT, &mTouchFrontData, 1);
			sceTouchPeek(SCE_TOUCH_PORT_BACK, &mTouchBackData, 1);
			sceCtrlPeekBufferPositive(0, &mPadData, 1);

			if (mPendingState != 0) {
				sceDisplayWaitSetFrameBuf();
				mCurrentState->exit();
				mCurrentState = mPendingState;
				mPendingState = 0;

				if (! mCurrentState->enter()) {
					return STATE_INITIALIZE;
				}
			}
			mCurrentState->update(mPadData, mTouchFrontData, mTouchBackData, mElapsed);
			return NO_ERROR;
		}

		bool Director::isRunning() const {
			return !mQuit;
		}

		void Director::stop() {
			mCurrentState->exit();
			mQuit = true;
		}

		int Director::changeState(const std::string name) {
			if (mGameStates.find(name) != mGameStates.end()) {
				mPendingState = mGameStates[name];
				return NO_ERROR;
			} else {
				return STATE_NOT_FOUND;
			}
		}

		unsigned int Director::getFPS() const {
			return mFps;
		}

	} // namespace Game

} // namespace NotEngine
