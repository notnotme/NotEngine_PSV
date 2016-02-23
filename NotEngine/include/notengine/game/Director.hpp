#ifndef DIRECTOR_HPP
#define	DIRECTOR_HPP

#pragma once
#include <map>
#include <string>
#include <psp2/ctrl.h>
#include <psp2/touch.h>

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

				uint64_t mCurrentTicks;
				uint64_t mLastFpsTicks;
				uint64_t mLastTicks;
				float mElapsed;
				unsigned int mFps;
				unsigned int mFrames;
				bool mQuit;

				Director();

			public:
				virtual ~Director();

				bool initialize (std::map<std::string, GameState*> states, const std::string start);
				void finalize () const;
				void update ();
				bool isRunning() const;
				void stop();
				bool changeState(const std::string name);
				unsigned int getFPS() const;
		};

	} // namespace Game

} // namespace NotEngine

#endif /* DIRECTOR_HPP */
