#include "../../include/notengine/graphics/Texture2D.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace NotEngine {

	namespace Graphics {

		Texture2D::Texture2D() {
			printf("Texture2D()\n");
		}

		Texture2D::~Texture2D() {
			printf("~Texture2D()\n");
		}

		bool Texture2D::initialize(unsigned int w, unsigned int h, SceGxmTextureFormat fmt) {
			mWidth = w;
			mHeight = h;
			mFormat = fmt;

			mTextureMemoryUID = 0;
			unsigned int storageSize =  w * h * getStorageSize(fmt);
			unsigned int* textureData = (unsigned int*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
				storageSize,
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mTextureMemoryUID);
			if (textureData == 0) {
				printf("Texture2D::initialize fail at gpuAlloc\n");
				return false;
			}

			memset(textureData, 0xff, storageSize);
			sceGxmTextureInitLinear(
				&mTexture,
				textureData, fmt,
				w, h, 0);

			return true;
		}

		void Texture2D::finalize() {
			if (mTextureMemoryUID != 0)
				GraphicsBase::gpuFree(mTextureMemoryUID);
		}

		void* Texture2D::getDataPtr() {
				if (mTextureMemoryUID != 0)
					return sceGxmTextureGetData(&mTexture);

				return 0;
		}

		unsigned int Texture2D::getStorageSize(SceGxmTextureFormat format) {
			switch (format & 0x9f000000U) {
				case SCE_GXM_TEXTURE_BASE_FORMAT_U8:
				case SCE_GXM_TEXTURE_BASE_FORMAT_S8:
					return 1;
				case SCE_GXM_TEXTURE_BASE_FORMAT_U4U4U4U4:
				case SCE_GXM_TEXTURE_BASE_FORMAT_U8U3U3U2:
				case SCE_GXM_TEXTURE_BASE_FORMAT_U1U5U5U5:
				case SCE_GXM_TEXTURE_BASE_FORMAT_U5U6U5:
				case SCE_GXM_TEXTURE_BASE_FORMAT_S5S5U6:
				case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8:
				case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8:
					return 2;
				case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8:
				case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8:
					return 3;
				case SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8:
				case SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8S8:
				case SCE_GXM_TEXTURE_BASE_FORMAT_F32:
				case SCE_GXM_TEXTURE_BASE_FORMAT_U32:
				case SCE_GXM_TEXTURE_BASE_FORMAT_S32:
				default:
					return 4;
			}
		}

	} // namespace Graphics

} // namespace NotEngine
