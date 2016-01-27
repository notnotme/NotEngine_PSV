#ifndef SPRITELETTER_HPP
#define	SPRITELETTER_HPP

#pragma once
#include <string>

#include "Sprite.hpp"
#include "FrameCatalog.hpp"

namespace NotEngine {

	namespace Graphics {

		// Base class to render sprite text
		class SpriteLetter : public Sprite {
			private:
				FrameCatalog::Frame* mFrames;

			public:
				SpriteLetter();
				virtual ~SpriteLetter();

				void setFrame(const char letter);
		};

	} // namespace Graphics

} // namespace NotEngine

#endif /* SPRITELETTER_HPP */
