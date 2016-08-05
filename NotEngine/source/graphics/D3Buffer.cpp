#include "../../include/notengine/graphics/D3Buffer.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"
#include "../include/notengine/graphics/Graphics3D.hpp"

#include <cstdlib>
#include <cstdio>

namespace NotEngine {

	namespace Graphics {

		D3Buffer::D3Buffer() {
		}

		D3Buffer::~D3Buffer() {
		}

		bool D3Buffer::initialize(unsigned int capacity, bool dynamic) {
			if (capacity > Graphics3D::MAX_VERTICES_PER_BATCH) {
				//printf("D3Buffer size can't be > than %i\n", Graphics3D::MAX_VERTICES_PER_BATCH);
				return false;
			}
			mVerticesCount = 0;
			mVerticesCapacity = capacity;
			mDynamic = dynamic;

			// Allocate buffers
			mBatchVertices = (D3Vertice*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				capacity*(sizeof(D3Vertice)),
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mBatchVerticesUID);

			return mBatchVertices != 0;
		}

		void D3Buffer::finalize() const {
			GraphicsBase::gpuFree(mBatchVerticesUID);
		}

		void D3Buffer::start() {
			mVerticesCount = 0;
			mVerticesOffset = 0;
		}

		void D3Buffer::put(float x, float y, float z, float s, float t, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			if (mVerticesOffset+mVerticesCount > mVerticesCapacity) {
				//printf("addToBatch discard. Capacity overflow\n");
				return;
			}

			mBatchVertices[mVerticesCount].x = x;
			mBatchVertices[mVerticesCount].y = y;
			mBatchVertices[mVerticesCount].z = z;
			mBatchVertices[mVerticesCount].s = s;
			mBatchVertices[mVerticesCount].t = t;
			mBatchVertices[mVerticesCount].r = r;
			mBatchVertices[mVerticesCount].g = g;
			mBatchVertices[mVerticesCount].b = b;
			mBatchVertices[mVerticesCount].a = a;
			mVerticesCount++;
		}

		void D3Buffer::put(const D3Vertice* vertice) {
			if (mVerticesOffset+mVerticesCount > mVerticesCapacity) {
				//printf("addToBatch discard. Capacity overflow\n");
				return;
			}

			mBatchVertices[mVerticesCount] = *vertice;
			mVerticesCount++;
		}

	} // namespace Graphics

} // namespace NotEngine
