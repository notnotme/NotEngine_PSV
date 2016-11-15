#include "../../include/notengine/graphics/D2Buffer.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"
#include "../include/notengine/graphics/Graphics2D.hpp"

namespace NotEngine {

	namespace Graphics {

		D2Buffer::D2Buffer() {
		}

		D2Buffer::~D2Buffer() {
		}

		int D2Buffer::initialize(unsigned int capacity, bool dynamic) {
			if (capacity > Graphics3D::MAX_VERTICES_PER_BATCH) {
				return SIZE_TO_BIG;
			}
			mVerticesCount = 0;
			mVerticesCapacity = capacity;
			mDynamic = dynamic;

			// Allocate buffers
			mBatchVertices = (D2Vertice*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				capacity*(sizeof(D2Vertice)),
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mBatchVerticesUID);

			return mBatchVertices != 0 ? NO_ERROR : VERTICES_GPU_ALLOC;
		}

		void D2Buffer::finalize() const {
			GraphicsBase::gpuFree(mBatchVerticesUID);
		}

		void D2Buffer::start() {
			mVerticesCount = 0;
			mVerticesOffset = 0;
		}

		int D2Buffer::put(float x, float y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			return put(x,y, .0f,.0f, r, g, b, a);
		}

		int D2Buffer::put(float x, float y, float s, float t, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			if (mVerticesOffset+mVerticesCount > mVerticesCapacity) {
				return BUFFER_OVERFLOW;
			}

			mBatchVertices[mVerticesCount].x = x;
			mBatchVertices[mVerticesCount].y = y;
			mBatchVertices[mVerticesCount].s = s;
			mBatchVertices[mVerticesCount].t = t;
			mBatchVertices[mVerticesCount].r = r;
			mBatchVertices[mVerticesCount].g = g;
			mBatchVertices[mVerticesCount].b = b;
			mBatchVertices[mVerticesCount].a = a;
			mVerticesCount++;
			return NO_ERROR;
		}

		int D2Buffer::put(const D2Vertice* vertice) {
			if (mVerticesOffset+mVerticesCount > mVerticesCapacity) {
				return BUFFER_OVERFLOW;
			}

			mBatchVertices[mVerticesCount] = *vertice;
			mVerticesCount++;
			return NO_ERROR;
		}

	} // namespace Graphics

} // namespace NotEngine
