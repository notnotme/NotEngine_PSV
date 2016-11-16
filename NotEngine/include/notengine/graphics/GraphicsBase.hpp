#ifndef GRAPHICSBASE_HPP
#define	GRAPHICSBASE_HPP

#pragma once
#include <psp2/types.h>
#include <psp2/display.h>
#include <psp2/gxm.h>
#include <psp2/kernel/sysmem.h>

#include "../system/Singleton.hpp"
#include "Texture2D.hpp"
#include "FrameCatalog.hpp"

namespace NotEngine {

	namespace Graphics {

		/**
		 * Base class that handle graphics stuff. All init and memalloc code was taken from xerpi's works.
		 * You can find more infos on his work at http://github.com/xerpi/
		 */
		class GraphicsBase : public System::Singleton<GraphicsBase> {
			friend class System::Singleton<GraphicsBase>;
			friend class Graphics2D;
			friend class Graphics3D;

			public:
				static const unsigned int DISPLAY_WIDTH = 960;
				static const unsigned int DISPLAY_HEIGHT = 544;
				static const unsigned int DISPLAY_STRIDE_IN_PIXELS = 1024;
				static const unsigned int DISPLAY_BUFFER_COUNT = 3;
				static const unsigned int DISPLAY_MAX_PENDING_SWAPS = 2;
				static const int DISPLAY_COLOR_FORMAT = SCE_GXM_COLOR_FORMAT_A8B8G8R8;
				static const int DISPLAY_PIXEL_FORMAT = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
				static const int MSAA_MODE = SCE_GXM_MULTISAMPLE_NONE;

				static const unsigned int DEBUG_FONT_CHAR_SIZE = 8;
				static const unsigned int DEBUG_FONT_TEXTURE_WIDTH = 128;
				static const unsigned int DEBUG_FONT_TEXTURE_HEIGHT = 128;
				static const SceGxmTextureFormat DEBUG_FONT_TEXTURE_FORMAT = SCE_GXM_TEXTURE_FORMAT_U4U4U4U4_ABGR;

			private:
				struct DisplayData {
					void* address;
					bool waitRetrace;
				} DisplayDatas;

				/// Disallow copy
				GraphicsBase(const GraphicsBase& copy);
				void operator=(GraphicsBase const&);

				SceUID mVdmRingBufferUid;
				SceUID mVertexRingBufferUid;
				SceUID mFragmentRingBufferUid;
				SceUID mFragmentUsseRingBufferUid;

				SceGxmContext* mContext;
				SceGxmContextParams mContextParams;
				SceGxmShaderPatcher* mShaderPatcher;
				SceGxmRenderTarget* mRenderTarget;

				// Todo: Duplicate and move those buffer into each Renderer for each uniform
				void *mVertexUniformDefaultBuffer;
				void *mFragmentUniformDefaultBuffer;
				SceUID mLastBatchVerticesUID;

				void* mDisplayBufferData[DISPLAY_BUFFER_COUNT];
				SceUID mDisplayBufferUid[DISPLAY_BUFFER_COUNT];
				SceGxmSyncObject* mDisplayBufferSync[DISPLAY_BUFFER_COUNT];

				SceGxmColorSurface mDisplaySurface[DISPLAY_BUFFER_COUNT];
				SceGxmDepthStencilSurface mDepthSurface;
				SceUID mDepthBufferUid;

				SceUID mPatcherBufferUid;
				SceUID mPatcherVertexUsseUid;
				SceUID mPatcherFragmentUsseUid;

				unsigned int mBackBufferIndex;
				unsigned int mFrontBufferIndex;
				bool mWaitRetrace;

				static void videoCallback(const void* userData);
				static void* patcherAlloc(void *userData, unsigned int size);
				static void patcherFree(void *userData, void *mem);

				static FrameCatalog::Frame* sFontFrames;
				static Graphics::Texture2D* sDebugFontTexture;


				/// Disallow public instanciating
				GraphicsBase ();

			public:
				virtual ~GraphicsBase ();

				/* INIT *****************************************************/
				/// Initialize the video subsystem
				int initialize(bool waitForRetrace);
				/// Terminate the rendering
				void waitTerminate() const;
				/// free the video subsystem
				void finalize();

				/* RENDER ***************************************************/
				/// Start rendreing into the default render target
				void startDrawing() const;
				/// End rendering
				void stopDrawing() const;
				/// Add the last buffer to the render queue
				void swapBuffers();
				/// Set wait for etrace enabled/disable
				void waitForRetrace(bool enable);

				/* HELPERS **************************************************/
				/// Gpu alloc helper
				static void* gpuAlloc(SceKernelMemBlockType type, unsigned int size, unsigned int attribs, SceUID *uid);
				/// Gpu free helper
				static void gpuFree(SceUID uid);
				/// Gpu vertex alloc
				static void* vertexUsseAlloc(unsigned int size, SceUID *uid, unsigned int *usse_offset);
				/// Gpu vertex free
				static void vertexUsseFree(SceUID uid);
				/// Gpu fragment alloc
				static void* fragmentUsseAlloc(unsigned int size, SceUID *uid, unsigned int *usse_offset);
				/// Gpu fragment free
				static void fragmentUsseFree(SceUID uid);
				/// align some memory to the desired alignement
				static unsigned int align(unsigned int number, unsigned int alignement);
				/// Return the frames to handle text drawing
				static FrameCatalog::Frame* getFontFrames();
				/// Return the texture 2d representating the debug font
				static Texture2D* getDebugFontTexture();

				enum ERROR {
					NO_ERROR = 0,
					SCEGXM_INITIALIZE = 1,
					SCEGXM_CREATE_CONTEXT = 2,
					SCEGXM_CREATE_RENDER_TARGET = 3,
					SCEGXM_COLOR_SURFACE_INIT = 4,
					SCEGXM_SYNC_OBJECT_CREATE = 5,
					SCEGXM_DEPTH_STENCIL_SURFACE_INIT = 6,
					SCEGXM_SHADER_PATCHER_CREATE = 7,
					DEBUGFONT_ALLOC_FAIL = 8,
					DEBUGFONT_TEXTURE_INITIALIZE = 9
				};

		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* GRAPHICSBASE_HPP */
