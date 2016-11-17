#include "../../include/notengine/graphics/SpriteBuffer.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"
#include "../include/notengine/graphics/Graphics2D.hpp"

namespace NotEngine {

	namespace Graphics {

		SpriteBuffer::SpriteBuffer() {
		}

		SpriteBuffer::~SpriteBuffer() {
		}

		int SpriteBuffer::initialize(unsigned int capacity, bool dynamic) {
			if (capacity > Graphics2D::MAX_SPRITES_PER_BATCH) {
				return SIZE_TO_BIG;
			}
			mBatchCount = 0;
			mBatchCapacity = capacity;
			mDynamic = dynamic;

			// Allocate buffers
			mBatchVertices = (SpriteVertice*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW,
				capacity*(sizeof(SpriteVertice)*4),
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mBatchVerticesUID);

			return mBatchVertices != 0 ? NO_ERROR : INDICES_GPU_ALLOC;
		}

		void SpriteBuffer::finalize() const {
			GraphicsBase::gpuFree(mBatchVerticesUID);
		}

		void SpriteBuffer::start() {
			mBatchCount = 0;
			mBatchOffset = 0;
		}

		int SpriteBuffer::put(const Sprite& sprite) {
			if (mBatchOffset+mBatchCount > mBatchCapacity) {
				return BUFFER_OVERFLOW;
			}

			unsigned int index = (mBatchCount)*4;
			float wdth = (sprite.frame.size.w/2) * sprite.scale.w;
			float hght = (sprite.frame.size.h/2) * sprite.scale.h;

			mBatchVertices[index].x = -wdth;
			mBatchVertices[index].y = -hght;
			mBatchVertices[index].s = sprite.frame.coords.s;
			mBatchVertices[index].t = sprite.frame.coords.t;
			mBatchVertices[index].r = sprite.color.r;
			mBatchVertices[index].g = sprite.color.g;
			mBatchVertices[index].b = sprite.color.b;
			mBatchVertices[index].a = sprite.color.a;
			mBatchVertices[index].angle = sprite.rotation;
			mBatchVertices[index].tx = sprite.position.x;
			mBatchVertices[index].ty = sprite.position.y;
			index++;

			mBatchVertices[index].x = wdth;
			mBatchVertices[index].y = -hght;
			mBatchVertices[index].s = sprite.frame.coords.u;
			mBatchVertices[index].t = sprite.frame.coords.t;
			mBatchVertices[index].r = sprite.color.r;
			mBatchVertices[index].g = sprite.color.g;
			mBatchVertices[index].b = sprite.color.b;
			mBatchVertices[index].a = sprite.color.a;
			mBatchVertices[index].angle = sprite.rotation;
			mBatchVertices[index].tx = sprite.position.x;
			mBatchVertices[index].ty = sprite.position.y;
			index++;

			mBatchVertices[index].x = wdth;
			mBatchVertices[index].y = hght;
			mBatchVertices[index].s = sprite.frame.coords.u;
			mBatchVertices[index].t = sprite.frame.coords.v;
			mBatchVertices[index].r = sprite.color.r;
			mBatchVertices[index].g = sprite.color.g;
			mBatchVertices[index].b = sprite.color.b;
			mBatchVertices[index].a = sprite.color.a;
			mBatchVertices[index].angle = sprite.rotation;
			mBatchVertices[index].tx = sprite.position.x;
			mBatchVertices[index].ty = sprite.position.y;
			index++;

			mBatchVertices[index].x = -wdth;
			mBatchVertices[index].y = hght;
			mBatchVertices[index].s = sprite.frame.coords.s;
			mBatchVertices[index].t = sprite.frame.coords.v;
			mBatchVertices[index].r = sprite.color.r;
			mBatchVertices[index].g = sprite.color.g;
			mBatchVertices[index].b = sprite.color.b;
			mBatchVertices[index].a = sprite.color.a;
			mBatchVertices[index].angle = sprite.rotation;
			mBatchVertices[index].tx = sprite.position.x;
			mBatchVertices[index].ty = sprite.position.y;

			mBatchCount++;
			return NO_ERROR;
		}

		int SpriteBuffer::put(float x, float y, int charOffset, SpriteLetter& sprite, const std::string text) {
			unsigned int offset = 0;
			sprite.position.y = y;
			sprite.position.x = x;
			int ret;
			for(std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
				sprite.setFrame(*it);
				ret = put(sprite);
				if (ret != NO_ERROR) {
					return ret;
				}
				sprite.position.x += sprite.frame.size.w + charOffset;
				offset++;
			}
			return NO_ERROR;
		}

	} // namespace Graphics

} // namespace NotEngine
