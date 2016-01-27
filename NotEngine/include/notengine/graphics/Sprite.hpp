#ifndef SPRITE_HPP
#define	SPRITE_HPP

#pragma once
#include "FrameCatalog.hpp"

namespace NotEngine {

	namespace Graphics {

		/** Base class for sprites */
		class Sprite {
			public:
				struct SpritePosition {
					float x,y;
				};
				struct SpriteScale {
					float w,h;
				};
				struct SpriteColor {
					unsigned char r,g,b,a;
				};

				FrameCatalog::Frame frame;
				SpritePosition position;
				SpriteScale scale;
				SpriteColor color;
				float rotation;

				Sprite();
				virtual ~Sprite();

		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* SPRITE_HPP */
