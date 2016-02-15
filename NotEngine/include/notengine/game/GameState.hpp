#ifndef GAMESTATE_HPP
#define	GAMESTATE_HPP

#pragma once
#include <string>
#include <psp2/ctrl.h>

namespace NotEngine {

	namespace Game {

		// Base class for different GameState
		class GameState {

			private:
				GameState(const GameState& copy);
				void operator=(GameState const&);

			public:
				GameState();
				virtual ~GameState();

				virtual bool enter () = 0;
				virtual void exit () = 0;

				virtual void update (const SceCtrlData* inputs, float elapsed) = 0;
				virtual const std::string getName() = 0;
		};

	} // namespace Game

} // namespace NotEngine

#endif /* GAMESTATE_HPP */
