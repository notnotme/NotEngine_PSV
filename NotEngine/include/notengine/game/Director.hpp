#ifndef DIRECTOR_HPP
#define	DIRECTOR_HPP

#pragma once
#include <map>
#include <string>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/rtc.h>

#include "../system/Singleton.hpp"
#include "GameState.hpp"

namespace NotEngine {

	namespace Game {

		// Class to manage differents GameState
		class Director : public System::Singleton<Director> {
			friend class System::Singleton<Director>;

			private:
				/// Disallow copy
				Director(const Director& copy);
				void operator=(Director const&);

				std::map<std::string, GameState*> mGameStates;
				GameState* mCurrentState;
				GameState* mPendingState;

				SceCtrlData mPadData;
				SceTouchData mTouchFrontData;
				SceTouchData mTouchBackData;

				SceRtcTick mCurrentTicks;
				SceRtcTick mLastFpsTicks;
				SceRtcTick mLastTicks;
				float mElapsed;
				unsigned int mFps;
				unsigned int mFrames;
				bool mQuit;

				Director();

			public:
				virtual ~Director();

				int initialize (std::map<std::string, GameState*> states, const std::string start);
				void finalize () const;
				int update ();
				bool isRunning() const;
				void stop();
				int changeState(const std::string name);
				unsigned int getFPS() const;

				enum ERROR {
					NO_ERROR = 0,
					STATE_INITIALIZE = 1,
					STATE_NOT_FOUND = 2
				};

		};

	} // namespace Game

} // namespace NotEngine

#endif /* DIRECTOR_HPP */
