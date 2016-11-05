#ifndef INDICEBUFFER_HPP
#define INDICEBUFFER_HPP

#pragma once
#include <climits>
#include <psp2/types.h>

namespace NotEngine {

	namespace Graphics {

		/* Base class that handle indices list */
		class IndiceBuffer {
			friend class Graphics2D;
			friend class Graphics3D;

			private:
				/// Disallow copy
				IndiceBuffer(const IndiceBuffer& copy);
				void operator=(IndiceBuffer const&);

				/// Total capacity of the batch per frame
				unsigned int mCapacity;
				/// Current offset of the indices
				unsigned int mOffset;
				/// Current indices count in the buffer
				unsigned int mCount;

				/// Pointer to use for datas
				unsigned short* mIndices;
				SceUID mIndicesUID;

			public:
				static const unsigned int MAX_INDICES = USHRT_MAX;

				IndiceBuffer();
				virtual ~IndiceBuffer();

				/// initialize the buffer with a capacity of [capacity] per frame
				int initialize(unsigned int capacity);
				/// clean up the buffer
				void finalize() const;
				/// Begin a new batch of indicess
				void start();
				/// Add an indice in the batch
				int put(unsigned short indice);

				enum ERROR {
					NO_ERROR = 0,
					SIZE_TO_BIG = 1,
					INDICES_GPU_ALLOC = 2,
					BUFFER_OVERFLOW = 3,
				};

			};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* INDICEBUFFER_HPP */
