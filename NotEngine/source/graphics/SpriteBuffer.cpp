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
			batchCount = 0;
			batchCapacity = capacity;

			// Allocate buffers
			batchVertices = (SpriteVertice*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				capacity*(sizeof(SpriteVertice)*4),
				SCE_GXM_MEMORY_ATTRIB_READ,
				&batchVerticesUID);

			return batchVertices != 0;
		}

		void SpriteBuffer::finalize() {
			GraphicsBase::gpuFree(batchVerticesUID);
		}

		void SpriteBuffer::start() {
			batchCount = 0;
		}

		void SpriteBuffer::put(const Graphics::Sprite* sprite) {
			if (batchCount >= batchCapacity) {
				printf("addToBatch discard. Capacity overflow\n");
				return;
			}

			unsigned int index = (batchCount)*4;
			float wdth = (sprite->size.w/2) * sprite->scale.w;
			float hght = (sprite->size.h/2) * sprite->scale.h;

			batchVertices[index].x = -wdth;
			batchVertices[index].y = -hght;
			batchVertices[index].s = sprite->frame.s;
			batchVertices[index].t = sprite->frame.t;
			batchVertices[index].r = sprite->color.r;
			batchVertices[index].g = sprite->color.g;
			batchVertices[index].b = sprite->color.b;
			batchVertices[index].a = sprite->color.a;
			batchVertices[index].angle = sprite->rotation;
			batchVertices[index].tx = sprite->position.x;
			batchVertices[index].ty = sprite->position.y;
			index++;

			batchVertices[index].x = wdth;
			batchVertices[index].y = -hght;
			batchVertices[index].s = sprite->frame.u;
			batchVertices[index].t = sprite->frame.t;
			batchVertices[index].r = sprite->color.r;
			batchVertices[index].g = sprite->color.g;
			batchVertices[index].b = sprite->color.b;
			batchVertices[index].a = sprite->color.a;
			batchVertices[index].angle = sprite->rotation;
			batchVertices[index].tx = sprite->position.x;
			batchVertices[index].ty = sprite->position.y;
			index++;

			batchVertices[index].x = wdth;
			batchVertices[index].y = hght;
			batchVertices[index].s = sprite->frame.u;
			batchVertices[index].t = sprite->frame.v;
			batchVertices[index].r = sprite->color.r;
			batchVertices[index].g = sprite->color.g;
			batchVertices[index].b = sprite->color.b;
			batchVertices[index].a = sprite->color.a;
			batchVertices[index].angle = sprite->rotation;
			batchVertices[index].tx = sprite->position.x;
			batchVertices[index].ty = sprite->position.y;
			index++;

			batchVertices[index].x = -wdth;
			batchVertices[index].y = hght;
			batchVertices[index].s = sprite->frame.s;
			batchVertices[index].t = sprite->frame.v;
			batchVertices[index].r = sprite->color.r;
			batchVertices[index].g = sprite->color.g;
			batchVertices[index].b = sprite->color.b;
			batchVertices[index].a = sprite->color.a;
			batchVertices[index].angle = sprite->rotation;
			batchVertices[index].tx = sprite->position.x;
			batchVertices[index].ty = sprite->position.y;

			batchCount++;
		}

	} // namespace Graphics

} // namespace NotEngine
