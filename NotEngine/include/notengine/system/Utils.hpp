#ifndef UTILS_HPP
#define	UTILS_HPP

#pragma once
#include <string>
#include <psp2/touch.h>

#include "../graphics/Sprite.hpp"

using namespace NotEngine::Graphics;

namespace NotEngine {

	namespace System {

		// Usefull things
		class Utils {

			private:
				Utils(const Utils& copy);
				void operator=(Utils const&);
				Utils();

			public:
				virtual ~Utils();

				/// Return the string content of a file
				static std::string stringFromFile(const std::string filename);
				static bool isTouched(int touchX, int touchY, const Sprite* sprite);
		};

	} // Namespace System

} // Namespace NotEngine

#endif /* UTILS_HPP */
