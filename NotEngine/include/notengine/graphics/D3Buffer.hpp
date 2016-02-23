#ifndef D3BUFFER_HPP
#define	D3BUFFER_HPP

#pragma once
#include <psp2/types.h>

namespace NotEngine {

	namespace Graphics {

		/* Base class that handle 3d drawing */
		class D3Buffer {
			friend class Graphics3D;

			public:
				/// Internal sprite vertice layout
				struct D3Vertice {
					float x; // vertices coords
					float y;
					float z;
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
				D3Buffer(const D3Buffer& copy);
				void operator=(D3Buffer const&);

				/// Total capacity of the vertices per frame
				unsigned int mVerticesCapacity;
				/// Current offset where to write more vertice in the buffer
				unsigned int mVerticesOffset;
				/// Current vertice count in the buffer
				unsigned int mVerticesCount;
				/// flag to set the buffer dynamic or not (data never change / can change)
				bool mDynamic;

				/// Pointer to use for verttice storage
				D3Vertice* mBatchVertices;
				SceUID mBatchVerticesUID;

			public:
				D3Buffer();
				virtual ~D3Buffer();

				/// initialize the buffer with a capacity of [capacity] per frame
				bool initialize(unsigned int capacity, bool dynamic);
				/// clean up the buffer
				void finalize() const;
				/// Reset the buffer and start drawing from index 0. Must be used maximum once per frame.
				void start();
				/// Add vertices in the batch
				void put(float x, float y, float z, float s, float t, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
				void put(const D3Vertice* vertice);
			};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* D3BUFFER_HPP */
