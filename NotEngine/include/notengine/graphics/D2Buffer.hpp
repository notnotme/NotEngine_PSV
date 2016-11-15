#ifndef D2BUFFER_HPP
#define	D2BUFFER_HPP

#pragma once
#include <psp2/types.h>

namespace NotEngine {

	namespace Graphics {

		/* A class that handle 2D shape drawing */
		class D2Buffer {
			friend class Graphics2D;

			public:
				/// Internal sprite vertice layout
				struct D2Vertice {
					float x; // vertices coords
					float y;
					float s; // tex coords
					float t;
					unsigned char r; // color
					unsigned char g;
					unsigned char b;
					unsigned char a;
					// todo: add padding ?
				} __attribute__ ((packed));

			private:
				/// Disallow copy
				D2Buffer(const D2Buffer& copy);
				void operator=(D2Buffer const&);

				/// Total capacity of the vertices per frame
				unsigned int mVerticesCapacity;
				/// Current offset where to write more vertice in the buffer
				unsigned int mVerticesOffset;
				/// Current vertice count in the buffer
				unsigned int mVerticesCount;
				/// flag to set the buffer dynamic or not (data never change / can change)
				bool mDynamic;

				/// Pointer to use for verttice storage
				D2Vertice* mBatchVertices;
				SceUID mBatchVerticesUID;

			public:
				D2Buffer();
				virtual ~D2Buffer();

				/// initialize the buffer with a capacity of [capacity] per frame
				int initialize(unsigned int capacity, bool dynamic);
				/// clean up the buffer
				void finalize() const;
				/// Reset the buffer and start drawing from index 0. Must be used maximum once per frame.
				void start();
				/// Add vertices in the batch
				int put(float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
				int put(float x, float y, float s, float t, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
				int put(const D2Vertice* vertice);

				enum ERROR {
					NO_ERROR = 0,
					SIZE_TO_BIG = 1,
					VERTICES_GPU_ALLOC = 2,
					BUFFER_OVERFLOW = 3,
				};

			};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* D2BUFFER_HPP */
