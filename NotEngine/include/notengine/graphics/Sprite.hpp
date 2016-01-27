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

				/// Store the information for one sprite frame
				FrameCatalog::Frame frame;
				/// Position of the sprite on screen
				SpritePosition position;
				/// scaling value
				SpriteScale scale;
				/// Color (not per vertex)
				SpriteColor color;
				/// angle of rotation
				float rotation;

				Sprite();
				virtual ~Sprite();

		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* SPRITE_HPP */
