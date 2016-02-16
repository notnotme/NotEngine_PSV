#ifndef SPRITEBUFFER_HPP
#define	SPRITEBUFFER_HPP

#pragma once
#include <psp2/types.h>

#include "Sprite.hpp"
#include "SpriteLetter.hpp"

namespace NotEngine {

	namespace Graphics {

		/* Base class that handle 2d sprites list */
		class SpriteBuffer {
			friend class Graphics2D;

			private:
				/// Internal sprite vertice layout
				struct SpriteVertice {
					float x; // vertices coords
					float y;
					float s; // tex coords
					float t;
					unsigned char r; // color
					unsigned char g;
					unsigned char b;
					unsigned char a;
					float angle; // angle
					float tx; // translation
					float ty;
				} __attribute__ ((packed));

				/// Disallow copy
				SpriteBuffer(const SpriteBuffer& copy);
				void operator=(SpriteBuffer const&);

				/// Total capacity of the batch per frame
				unsigned int mBatchCapacity;
				/// Current offset where to write sprite added in the buffer
				unsigned int mBatchOffset;
				/// Current sprite count in the buffer
				unsigned int mBatchCount;
				/// flag to set the buffer static or not (data change or never change)
				bool mDynamic;

				/// Pointer to use for sprite batching
				SpriteVertice* mBatchVertices;
				SceUID mBatchVerticesUID;

			public:
				SpriteBuffer();
				virtual ~SpriteBuffer();

				/// initialize the buffer with a capacity of [capacity] per frame
				bool initialize(unsigned int capacity, bool dynamic);
				/// clean up the buffer
				void finalize();
				/// Begin a new batch of sprites, should be called once per frame to reset the buffer offset
				void start();
				/// Add a sprite in the batch
				void put(const Graphics::Sprite* sprite);
				void put(float x, float y, int offset, Graphics::SpriteLetter* sprite, const std::string text);
			};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* SPRITEBUFFER_HPP */
