#include "../../include/notengine/graphics/SpriteBuffer.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"
#include "../include/notengine/graphics/Graphics2D.hpp"

#include <stdlib.h>
#include <stdio.h>

namespace NotEngine {

	namespace Graphics {

		SpriteBuffer::SpriteBuffer() {
			printf("SpriteBuffer()\n");
		}

		SpriteBuffer::~SpriteBuffer() {
			printf("~SpriteBuffer()\n");
		}

		bool SpriteBuffer::initialize(unsigned int capacity) {
			if (capacity >= Graphics2D::MAX_SPRITES_PER_BATCH) {
				printf("SpriteBuffer size can't be > than %i", Graphics2D::MAX_SPRITES_PER_BATCH);
				return false;
			}
			mBatchCount = 0;
			mBatchCapacity = capacity;

			// Allocate buffers
			mBatchVertices = (SpriteVertice*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				capacity*(sizeof(SpriteVertice)*4),
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mBatchVerticesUID);

			return mBatchVertices != 0;
		}

		void SpriteBuffer::finalize() {
			GraphicsBase::gpuFree(mBatchVerticesUID);
		}

		void SpriteBuffer::start() {
			mBatchCount = 0;
		}

		void SpriteBuffer::put(const Graphics::Sprite* sprite) {
			if (mBatchCount >= mBatchCapacity) {
				printf("addToBatch discard. Capacity overflow\n");
				return;
			}

			unsigned int index = (mBatchCount)*4;
			float wdth = (sprite->size.w/2) * sprite->scale.w;
			float hght = (sprite->size.h/2) * sprite->scale.h;

			mBatchVertices[index].x = -wdth;
			mBatchVertices[index].y = -hght;
			mBatchVertices[index].s = sprite->frame.s;
			mBatchVertices[index].t = sprite->frame.t;
			mBatchVertices[index].r = sprite->color.r;
			mBatchVertices[index].g = sprite->color.g;
			mBatchVertices[index].b = sprite->color.b;
			mBatchVertices[index].a = sprite->color.a;
			mBatchVertices[index].angle = sprite->rotation;
			mBatchVertices[index].tx = sprite->position.x;
			mBatchVertices[index].ty = sprite->position.y;
			index++;

			mBatchVertices[index].x = wdth;
			mBatchVertices[index].y = -hght;
			mBatchVertices[index].s = sprite->frame.u;
			mBatchVertices[index].t = sprite->frame.t;
			mBatchVertices[index].r = sprite->color.r;
			mBatchVertices[index].g = sprite->color.g;
			mBatchVertices[index].b = sprite->color.b;
			mBatchVertices[index].a = sprite->color.a;
			mBatchVertices[index].angle = sprite->rotation;
			mBatchVertices[index].tx = sprite->position.x;
			mBatchVertices[index].ty = sprite->position.y;
			index++;

			mBatchVertices[index].x = wdth;
			mBatchVertices[index].y = hght;
			mBatchVertices[index].s = sprite->frame.u;
			mBatchVertices[index].t = sprite->frame.v;
			mBatchVertices[index].r = sprite->color.r;
			mBatchVertices[index].g = sprite->color.g;
			mBatchVertices[index].b = sprite->color.b;
			mBatchVertices[index].a = sprite->color.a;
			mBatchVertices[index].angle = sprite->rotation;
			mBatchVertices[index].tx = sprite->position.x;
			mBatchVertices[index].ty = sprite->position.y;
			index++;

			mBatchVertices[index].x = -wdth;
			mBatchVertices[index].y = hght;
			mBatchVertices[index].s = sprite->frame.s;
			mBatchVertices[index].t = sprite->frame.v;
			mBatchVertices[index].r = sprite->color.r;
			mBatchVertices[index].g = sprite->color.g;
			mBatchVertices[index].b = sprite->color.b;
			mBatchVertices[index].a = sprite->color.a;
			mBatchVertices[index].angle = sprite->rotation;
			mBatchVertices[index].tx = sprite->position.x;
			mBatchVertices[index].ty = sprite->position.y;

			mBatchCount++;
		}

	} // namespace Graphics

} // namespace NotEngine
