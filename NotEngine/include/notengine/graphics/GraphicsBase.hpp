#ifndef GRAPHICSBASE_HPP
#define	GRAPHICSBASE_HPP

#pragma once
#include <psp2/types.h>
#include <psp2/display.h>
#include <psp2/gxm.h>
#include <psp2/kernel/sysmem.h>

#include "../system/Singleton.hpp"

using namespace NotEngine;

namespace NotEngine {

	namespace Graphics {

		/**
		 * Base class that handle graphics stuff. All init and memalloc code was taken from xerpi's works.
		 * You can find more infos on his work at http://github.com/xerpi/
		 */
		class GraphicsBase : public System::Singleton<GraphicsBase> {
			public:
				static const unsigned int DISPLAY_WIDTH = 960;
				static const unsigned int DISPLAY_HEIGHT = 544;
				static const unsigned int DISPLAY_STRIDE_IN_PIXELS = 1024;
				static const unsigned int DISPLAY_BUFFER_COUNT = 3;
				static const unsigned int DISPLAY_MAX_PENDING_SWAPS = 2;
				static const int DISPLAY_COLOR_FORMAT = SCE_GXM_COLOR_FORMAT_A8B8G8R8;
				static const int DISPLAY_PIXEL_FORMAT = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
				static const int MSAA_MODE = SCE_GXM_MULTISAMPLE_NONE;

			private:
				/// Disallow copy
				GraphicsBase(const GraphicsBase& copy);
				void operator=(GraphicsBase const&);

				SceUID vdmRingBufferUid;
				SceUID vertexRingBufferUid;
				SceUID fragmentRingBufferUid;
				SceUID fragmentUsseRingBufferUid;

				SceGxmContextParams contextParams;
				SceGxmRenderTarget* renderTarget;
				SceGxmDepthStencilSurface depthSurface;
				SceUID depthBufferUid;

				void* displayBufferData[DISPLAY_BUFFER_COUNT];
				SceUID displayBufferUid[DISPLAY_BUFFER_COUNT];
				SceGxmColorSurface displaySurface[DISPLAY_BUFFER_COUNT];
				SceGxmSyncObject* displayBufferSync[DISPLAY_BUFFER_COUNT];

				SceUID patcherBufferUid;
				SceUID patcherVertexUsseUid;
				SceUID patcherFragmentUsseUid;

				unsigned int backBufferIndex;
				unsigned int frontBufferIndex;
				bool waitRetrace;

				static void videoCallback(const void* userData);
				static void* patcherAlloc(void *userData, unsigned int size);
				static void patcherFree(void *userData, void *mem);

				struct DisplayData {
					void* address;
					bool waitRetrace;
				} DisplayDatas;

			protected:
				/// Friend class itself
				friend class System::Singleton<GraphicsBase>;
				friend class Graphics2D;

				/// Disallow public instanciating
				GraphicsBase ();

				SceGxmContext* context;
				SceGxmShaderPatcher* shaderPatcher;

			public:
				virtual ~GraphicsBase ();

				/* INIT *****************************************************/
				/// Initialize the video subsystem
				bool initialize(bool waitForRetrace);
				/// free the video subsystem
				void finalize();

				/* RENDER ***************************************************/
				/// Start rendreing into the default render target
				void startDrawing();
				/// End rendering
				void stopDrawing();
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

				static unsigned int align(unsigned int number, unsigned int alignement);
		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* GRAPHICSBASE_HPP */