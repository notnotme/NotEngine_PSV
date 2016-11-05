#include "../../include/notengine/graphics/IndiceBuffer.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"
#include "../include/notengine/graphics/Graphics2D.hpp"
#include "../include/notengine/graphics/Graphics3D.hpp"

namespace NotEngine {

	namespace Graphics {

		IndiceBuffer::IndiceBuffer() {
		}

		IndiceBuffer::~IndiceBuffer() {
		}

		int IndiceBuffer::initialize(unsigned int capacity) {
			if (capacity > IndiceBuffer::MAX_INDICES) {
				return SIZE_TO_BIG;
			}
			mCount = 0;
			mCapacity = capacity;

			// Allocate buffers
			mIndicesUID = 0;
			mIndices = (unsigned short*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				capacity * sizeof(unsigned short),
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mIndicesUID);

			return mIndices != 0 ? NO_ERROR : INDICES_GPU_ALLOC;
		}

		void IndiceBuffer::finalize() const {
			GraphicsBase::gpuFree(mIndicesUID);
		}

		void IndiceBuffer::start() {
			mCount = 0;
			mOffset = 0;
		}

		int IndiceBuffer::put(const unsigned short indice) {
			if (mOffset+mCount > mCapacity) {
				return BUFFER_OVERFLOW;
			}

			mIndices[mCount] = indice;
			mCount++;

			return NO_ERROR;
		}

	} // namespace Graphics

} // namespace NotEngine
