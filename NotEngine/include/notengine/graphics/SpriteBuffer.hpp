#ifndef SPRITEBUFFER_HPP
#define	SPRITEBUFFER_HPP

#pragma once
#include <psp2/types.h>

#include "Sprite.hpp"

using namespace NotEngine;

namespace NotEngine {

	namespace Graphics {

		/**
		 * Base class that handle 2d sprites list.
		 */
		class SpriteBuffer {
			friend class Graphics2D;

			private:
				/// Disallow copy
				SpriteBuffer(const SpriteBuffer& copy);
				void operator=(SpriteBuffer const&);

				unsigned int batchCapacity;
				unsigned int batchCount;

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

			public:
				/// Pointer to use for sprite batching
				SpriteVertice* batchVertices;
				SceUID batchVerticesUID;

				SpriteBuffer();
				virtual ~SpriteBuffer();

				/// initialize the buffer
				bool initialize(unsigned int capacity);
				/// clean up the buffer
				void finalize();
				/// Begin a new batch of sprites
				void start();
				/// Add a sprite in the batch
				void put(const Graphics::Sprite* sprite);
			};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* SPRITEBUFFER_HPP */
