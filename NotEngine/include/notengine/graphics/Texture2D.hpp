#ifndef TEXTURE2D_HPP
#define	TEXTURE2D_HPP

#pragma once
#include <psp2/types.h>
#include <psp2/gxm.h>

namespace NotEngine {

	namespace Graphics {

		/**
		 * Base class that handle 2d graphics stuff.
		 */
		class Texture2D {
			friend class Graphics2D;
			friend class Graphics3D;

			private:
				/// Disallow copy
				Texture2D(const Texture2D& copy);
				void operator=(Texture2D const&);

				/// The GXM Texture object
				SceGxmTexture mTexture;
				SceUID mTextureMemoryUID;

				/// Width of the texture
				unsigned int mWidth;
				/// Height of the texture
				unsigned int mHeight;
				/// Format of the texture
				SceGxmTextureFormat mFormat;

			public:
				Texture2D ();
				virtual ~Texture2D ();

				/* INIT **************************************************/
				/// Initialize a texture
				bool initialize(unsigned int w, unsigned int h, SceGxmTextureFormat format);
				/// finalize a texture
				void finalize() const;
				/// Return a pointer to the datas (RW)
				void* getDataPtr() const;
				/// Set the texture filter
				void setFilter(SceGxmTextureFilter min, SceGxmTextureFilter mag);
				/// Set the texture repeat mode
				void setWrap(SceGxmTextureAddrMode u, SceGxmTextureAddrMode v);
				/// Enable mipmap filtering
				void setMipFilter(SceGxmTextureMipFilter filter);
				/// return the size of one pixel of the current format
				static unsigned int getStorageSize(SceGxmTextureFormat format);
				/// Return the rounded size of a line of pixel for a texture
				static unsigned int getStride(unsigned int width, SceGxmTextureFormat fmt);
		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* TEXTURE2D_HPP */
