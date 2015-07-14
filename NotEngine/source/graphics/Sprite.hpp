#ifndef SPRITE_HPP
#define	SPRITE_HPP

#pragma once

namespace NotEngine {

	namespace Graphics {

		/** Base class for sprites */
		class Sprite {

			public:
				struct SpritePosition {
					float x,y;
				};
				struct SpriteSize {
					unsigned int w,h;
				};
				struct SpriteFrame {
					float s,t,u,v;
				};
				struct SpriteScale {
					float w,h;
				};
				struct SpriteColor {
					unsigned char r,g,b,a;
				};

				SpritePosition position;
				SpriteSize size;
				SpriteFrame frame;
				SpriteScale scale;
				SpriteColor color;
				float rotation;

				Sprite() :
					position((SpritePosition) {0,0}),
					size((SpriteSize){64,64}),
					frame((SpriteFrame){0,0,1,1}),
					scale((SpriteScale){1,1}),
					color((SpriteColor) {0xff, 0xff, 0xff, 0xff}),
					rotation(0) {
				};

				virtual ~Sprite(){
				};

		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* SPRITE_HPP */
