#include "../../include/notengine/graphics/GraphicsBase.hpp"
#include "../../datas/debugfont.raw.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace NotEngine {

	namespace Graphics {

		FrameCatalog::Frame* GraphicsBase::sFontFrames = 0;
		Graphics::Texture2D* GraphicsBase::sDebugFontTexture = 0;

		GraphicsBase::GraphicsBase() : System::Singleton<GraphicsBase>() {
			printf("GraphicsBase()\n");
		}

		GraphicsBase::~GraphicsBase() {
			printf("~GraphicsBase()\n");
		}

		void* GraphicsBase::patcherAlloc(void *userData, unsigned int size) {
			return malloc(size);
		}

		void GraphicsBase::patcherFree(void *userData, void *mem) {
			free(mem);
		}

		void GraphicsBase::waitForRetrace(bool enabled) {
			mWaitRetrace = enabled;
		}

		void* GraphicsBase::gpuAlloc(SceKernelMemBlockType type, unsigned int size, unsigned int attribs, SceUID* uid) {
			void *mem = 0;

			if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW) {
				// CDRAM memblocks must be 256KiB aligned
				size = align(size, 256*1024);
			} else {
				// LPDDR memblocks must be 4KiB aligned
				size = align(size, 4*1024);
			}

			*uid = sceKernelAllocMemBlock("gpu_mem", type, size, 0);
			if (sceKernelGetMemBlockBase(*uid, &mem) < 0)
				return 0;

			if (sceGxmMapMemory(mem, size, (SceGxmMemoryAttribFlags) attribs) < 0)
				return 0;

			return mem;
		}

		void GraphicsBase::gpuFree(SceUID uid) {
			void *mem = 0;
			if (sceKernelGetMemBlockBase(uid, &mem) < 0) {
				return;
			}
			sceGxmUnmapMemory(mem);
			sceKernelFreeMemBlock(uid);
		}

		void* GraphicsBase::vertexUsseAlloc(unsigned int size, SceUID* uid, unsigned int* usse_offset) {
			void *mem = 0;

			size = align(size, 4096);
			*uid = sceKernelAllocMemBlock("vertex_usse", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, 0);
			if (sceKernelGetMemBlockBase(*uid, &mem) < 0) {
				return 0;
			}
			if (sceGxmMapVertexUsseMemory(mem, size, usse_offset) < 0) {
				return 0;
			}

			return mem;
		}

		void GraphicsBase::vertexUsseFree(SceUID uid)
		{
			void *mem = 0;
			if (sceKernelGetMemBlockBase(uid, &mem) < 0) {
				return;
			}
			sceGxmUnmapVertexUsseMemory(mem);
			sceKernelFreeMemBlock(uid);
		}

		void* GraphicsBase::fragmentUsseAlloc(unsigned int size, SceUID *uid, unsigned int *usse_offset) {
			void *mem = 0;

			size = align(size, 4096);
			*uid = sceKernelAllocMemBlock("fragment_usse", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, size, 0);
			if (sceKernelGetMemBlockBase(*uid, &mem) < 0) {
				return 0;
			}
			if (sceGxmMapFragmentUsseMemory(mem, size, usse_offset) < 0) {
				return 0;
			}

			return mem;
		}

		void GraphicsBase::fragmentUsseFree(SceUID uid) {
			void *mem = 0;
			if (sceKernelGetMemBlockBase(uid, &mem) < 0) {
				return;
			}
			sceGxmUnmapFragmentUsseMemory(mem);
			sceKernelFreeMemBlock(uid);
		}

		unsigned int GraphicsBase::align(unsigned int number, unsigned int alignement) {
			return (((number) + ((alignement) - 1)) & ~((alignement) - 1));
		}

		FrameCatalog::Frame* GraphicsBase::getFontFrames() {
			return sFontFrames;
		}

		Texture2D* GraphicsBase::getDebugFontTexture() {
			return sDebugFontTexture;
		}

		void GraphicsBase::videoCallback(const void* userData) {
			const DisplayData* displayData = (const DisplayData*) userData;

			SceDisplayFrameBuf framebuf = (SceDisplayFrameBuf) {
				.size = sizeof(SceDisplayFrameBuf),
				.base = displayData->address,
				.pitch = GraphicsBase::DISPLAY_STRIDE_IN_PIXELS,
				.pixelformat = GraphicsBase::DISPLAY_PIXEL_FORMAT,
				.width = GraphicsBase::DISPLAY_WIDTH,
				.height = GraphicsBase::DISPLAY_HEIGHT
			};
			sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);

			if (displayData->waitRetrace) {
				sceDisplayWaitVblankStart();
			}
		}

		bool GraphicsBase::initialize(bool waitForRetrace) {
			mWaitRetrace = waitForRetrace;
			SceGxmInitializeParams initializeParams = (SceGxmInitializeParams) {
				.flags = 0,
				.displayQueueMaxPendingCount = GraphicsBase::DISPLAY_MAX_PENDING_SWAPS,
				.displayQueueCallback = videoCallback,
				.displayQueueCallbackDataSize = sizeof(DisplayData),
				.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE
			};

			int err = sceGxmInitialize(&initializeParams);
			if (err != 0) {
				printf("sceGxmInitialize(): 0x%08X\n", err);
				return false;
			}

			// allocate ring buffer memory using default sizes
			void* vdmRingBuffer = 0;
			vdmRingBuffer = GraphicsBase::gpuAlloc(
				(SceKernelMemBlockType) SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				(unsigned int) SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mVdmRingBufferUid);

			void* vertexRingBuffer = 0;
			vertexRingBuffer = GraphicsBase::gpuAlloc(
				(SceKernelMemBlockType) SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				(unsigned int) SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mVertexRingBufferUid);

			void* fragmentRingBuffer = 0;
			fragmentRingBuffer = GraphicsBase::gpuAlloc(
				(SceKernelMemBlockType) SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				(unsigned int) SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mFragmentRingBufferUid);

			unsigned int fragmentUsseRingBufferOffset;
			void* fragmentUsseRingBuffer = 0;
			fragmentUsseRingBuffer = GraphicsBase::fragmentUsseAlloc(
				(unsigned int)  SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,
				&mFragmentUsseRingBufferUid,
				&fragmentUsseRingBufferOffset);

			mContext = 0;
			mContextParams = (SceGxmContextParams) {
				.hostMem = malloc(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE),
				.hostMemSize = SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE,
				.vdmRingBufferMem = vdmRingBuffer,
				.vdmRingBufferMemSize = SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,
				.vertexRingBufferMem = vertexRingBuffer,
				.vertexRingBufferMemSize = SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,
				.fragmentRingBufferMem = fragmentRingBuffer,
				.fragmentRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,
				.fragmentUsseRingBufferMem = fragmentUsseRingBuffer,
				.fragmentUsseRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,
				.fragmentUsseRingBufferOffset = fragmentUsseRingBufferOffset
			};
			err = sceGxmCreateContext(&mContextParams, &mContext);
			if(err != 0) {
				printf("sceGxmCreateContext(): 0x%08X\n", err);
				return false;
			}

			// create the render target
			SceGxmRenderTargetParams renderTargetParams = (SceGxmRenderTargetParams) {
				.flags = 0,
				.width = DISPLAY_WIDTH,
				.height = DISPLAY_HEIGHT,
				.scenesPerFrame = 1,
				.multisampleMode = MSAA_MODE,
				.multisampleLocations = 0,
				.driverMemBlock = -1 // Invalid UID
			};
			err = sceGxmCreateRenderTarget(&renderTargetParams, &mRenderTarget);
			if (err != 0) {
	 			printf("sceGxmCreateRenderTarget(): 0x%08X\n", err);
	 			return false;
			}

			// allocate memory and sync objects for display buffers
			for (unsigned int i=0; i<GraphicsBase::DISPLAY_BUFFER_COUNT; i++) {
				// initialize a color surface for this display buffer
				mDisplayBufferData[i] = 0;
				mDisplayBufferData[i] = GraphicsBase::gpuAlloc(
					SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
					4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT,
					SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
					&mDisplayBufferUid[i]);

				memset(mDisplayBufferData[i], 0xff000000, 4*GraphicsBase::DISPLAY_STRIDE_IN_PIXELS*GraphicsBase::DISPLAY_HEIGHT);
				err = sceGxmColorSurfaceInit(
					&mDisplaySurface[i],
					(SceGxmColorFormat) DISPLAY_COLOR_FORMAT,
					SCE_GXM_COLOR_SURFACE_LINEAR,
					(MSAA_MODE == SCE_GXM_MULTISAMPLE_NONE) ? SCE_GXM_COLOR_SURFACE_SCALE_NONE : SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE,
					SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
					GraphicsBase::DISPLAY_WIDTH,
					GraphicsBase::DISPLAY_HEIGHT,
					GraphicsBase::DISPLAY_STRIDE_IN_PIXELS,
					mDisplayBufferData[i]);
				if (err != 0) {
		 			printf("sceGxmColorSurfaceInit(): 0x%08X\n", err);
		 			return false;
		 		}

				// create a sync object that we will associate with this buffer
				err = sceGxmSyncObjectCreate(&mDisplayBufferSync[i]);
				if (err != 0) {
		 			printf("sceGxmSyncObjectCreate(): 0x%08X\n", err);
		 			return false;
				}
			}

			// compute the memory footprint of the depth buffer
			const unsigned int alignedWidth = GraphicsBase::align(GraphicsBase::DISPLAY_WIDTH, SCE_GXM_TILE_SIZEX);
			const unsigned int alignedHeight = GraphicsBase::align(GraphicsBase::DISPLAY_HEIGHT, SCE_GXM_TILE_SIZEY);
			unsigned int sampleCount = alignedWidth*alignedHeight;
			unsigned int depthStrideInSamples = alignedWidth;
			if (MSAA_MODE == SCE_GXM_MULTISAMPLE_4X) {
				// samples increase in X and Y
				sampleCount *= 4;
				depthStrideInSamples *= 2;
			} else if (MSAA_MODE == SCE_GXM_MULTISAMPLE_2X) {
				// samples increase in Y only
				sampleCount *= 2;
			}

			// create the SceGxmDepthStencilSurface structure
			void* depthBufferData = 0;
			depthBufferData = GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				4*sampleCount,
				SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
				&mDepthBufferUid);

			err = sceGxmDepthStencilSurfaceInit(
				&mDepthSurface,
				SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24,
				SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
				depthStrideInSamples,
				depthBufferData,
				0);
			if (err != 0) {
	 			printf("sceGxmDepthStencilSurfaceInit(): 0x%08X\n", err);
	 			return false;
			}

			// allocate memory for buffers and USSE code
			const unsigned int patcherBufferSize		= 64*1024;
			const unsigned int patcherVertexUsseSize	= 64*1024;
			const unsigned int patcherFragmentUsseSize	= 64*1024;

			void* patcherBuffer = 0;
			patcherBuffer = GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				patcherBufferSize,
				SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
				&mPatcherBufferUid);

			unsigned int patcherVertexUsseOffset;
			void* patcherVertexUsse = 0;
			patcherVertexUsse = GraphicsBase::vertexUsseAlloc(
				patcherVertexUsseSize,
				&mPatcherVertexUsseUid,
				&patcherVertexUsseOffset);

			unsigned int patcherFragmentUsseOffset;
			void* patcherFragmentUsse = 0;
			patcherFragmentUsse = GraphicsBase::fragmentUsseAlloc(
				patcherFragmentUsseSize,
				&mPatcherFragmentUsseUid,
				&patcherFragmentUsseOffset);

			// create a shader patcher
			SceGxmShaderPatcherParams patcherParams = (SceGxmShaderPatcherParams) {
				.userData = 0,
				.hostAllocCallback = &patcherAlloc,
				.hostFreeCallback = &patcherFree,
				.bufferAllocCallback = 0,
				.bufferFreeCallback = 0,
				.bufferMem = patcherBuffer,
				.bufferMemSize = patcherBufferSize,
				.vertexUsseAllocCallback = 0,
				.vertexUsseFreeCallback = 0,
				.vertexUsseMem	 = patcherVertexUsse,
				.vertexUsseMemSize = patcherVertexUsseSize,
				.vertexUsseOffset = patcherVertexUsseOffset,
				.fragmentUsseAllocCallback = 0,
				.fragmentUsseFreeCallback = 0,
				.fragmentUsseMem = patcherFragmentUsse,
				.fragmentUsseMemSize = patcherFragmentUsseSize,
				.fragmentUsseOffset = patcherFragmentUsseOffset
			};
			err = sceGxmShaderPatcherCreate(&patcherParams, &mShaderPatcher);
			if (err != 0) {
				printf("sceGxmShaderPatcherCreate(): 0x%08X\n", err);
				return false;
			}

			// Allocate frames for fonts
			sFontFrames = 0;
			sFontFrames = new FrameCatalog::Frame[256];
			if (sFontFrames == 0) {
				printf("GraphicsBase can't allocate frames for font\n");
				return false;
			}

			unsigned int offset = 0;
			for (float y=0.0f; y<128.0f; y+=8.0f) {
				for (float x=0.0f; x<128.0f; x+=8.0f) {
					FrameCatalog::Frame* frame = &sFontFrames[offset];
					frame->coords.s = (x / (float) GraphicsBase::DEBUG_FONT_TEXTURE_WIDTH);
					frame->coords.t = (y / (float) GraphicsBase::DEBUG_FONT_TEXTURE_HEIGHT);
					frame->coords.u = ((x + (float)GraphicsBase::DEBUG_FONT_CHAR_SIZE) / (float) GraphicsBase::DEBUG_FONT_TEXTURE_WIDTH);
					frame->coords.v = ((y + (float) GraphicsBase::DEBUG_FONT_CHAR_SIZE) / (float) GraphicsBase::DEBUG_FONT_TEXTURE_HEIGHT);
					frame->size.w = GraphicsBase::DEBUG_FONT_CHAR_SIZE;
					frame->size.h = GraphicsBase::DEBUG_FONT_CHAR_SIZE;
					offset++;
				}
			}

			// Create debug font texture
			sDebugFontTexture = 0;
			sDebugFontTexture = new Texture2D();
			if (!sDebugFontTexture->initialize(GraphicsBase::DEBUG_FONT_TEXTURE_WIDTH, GraphicsBase::DEBUG_FONT_TEXTURE_HEIGHT, GraphicsBase::DEBUG_FONT_TEXTURE_FORMAT)) {
				printf("GraphicsBase failed to create debug font texture\n");
				return false;
			}
			char* buffer = (char*)  sDebugFontTexture->getDataPtr();
			memcpy(buffer, &DEBUGFONT_RAW[0], DEBUGFONT_RAW_SIZE);

			sceGxmSetCullMode(mContext, SCE_GXM_CULL_CW);
			sceGxmSetTwoSidedEnable(mContext, SCE_GXM_TWO_SIDED_DISABLED);
			sceGxmSetFrontDepthFunc(mContext, SCE_GXM_DEPTH_FUNC_LESS);
			//sceGxmSetFrontFragmentProgramEnable(mContext, SCE_GXM_FRAGMENT_PROGRAM_ENABLED);
			//sceGxmSetFrontPolygonMode(mContext, SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);

			mBackBufferIndex = 0;
			mFrontBufferIndex = 0;

			return true;
		}

		void GraphicsBase::waitTerminate() {
			// wait until rendering is done
			if (mContext != 0) {
				sceGxmFinish(mContext);
				sceGxmDisplayQueueFinish();
			}
		}

		void GraphicsBase::finalize() {
			if (sDebugFontTexture != 0) {
				sDebugFontTexture->finalize();
				delete sDebugFontTexture;
			}

			if (sFontFrames != 0)
				delete [] sFontFrames;

			if (mDepthBufferUid != 0)
				GraphicsBase::gpuFree(mDepthBufferUid);

			for (unsigned int i=0; i<GraphicsBase::DISPLAY_BUFFER_COUNT; i++) {
				// clear the buffer then deallocate
				if (mDisplayBufferUid[i] != 0) {
					memset(mDisplayBufferData[i], 0, GraphicsBase::DISPLAY_HEIGHT*GraphicsBase::DISPLAY_STRIDE_IN_PIXELS*4);
					GraphicsBase::gpuFree(mDisplayBufferUid[i]);
					sceGxmSyncObjectDestroy(mDisplayBufferSync[i]);
				}
			}

			if (mRenderTarget != 0)
				sceGxmDestroyRenderTarget(mRenderTarget);

			if (mShaderPatcher != 0)
				sceGxmShaderPatcherDestroy(mShaderPatcher);

			if (mPatcherFragmentUsseUid != 0)
				GraphicsBase::fragmentUsseFree(mPatcherFragmentUsseUid);

			if (mPatcherVertexUsseUid != 0)
				GraphicsBase::vertexUsseFree(mPatcherVertexUsseUid);

			if (mPatcherBufferUid != 0)
				GraphicsBase::gpuFree(mPatcherBufferUid);

			if (mContext != 0)
				sceGxmDestroyContext(mContext);

			if (mFragmentUsseRingBufferUid != 0)
				GraphicsBase::fragmentUsseFree(mFragmentUsseRingBufferUid);

			if (mFragmentRingBufferUid != 0)
				GraphicsBase::gpuFree(mFragmentRingBufferUid);

			if (mVertexRingBufferUid != 0)
				GraphicsBase::gpuFree(mVertexRingBufferUid);

			if (mVdmRingBufferUid != 0)
				GraphicsBase::gpuFree(mVdmRingBufferUid);

			if (mContextParams.hostMem != 0)
				free(mContextParams.hostMem);

			// terminate libgxm
			sceGxmTerminate();
		}

		void GraphicsBase::swapBuffers() {
			sceGxmPadHeartbeat(&mDisplaySurface[mBackBufferIndex], mDisplayBufferSync[mBackBufferIndex]);

			// queue the display swap for this frame
			DisplayData displayData = (DisplayData) {
				.address = mDisplayBufferData[mBackBufferIndex],
				.waitRetrace = mWaitRetrace
			};
			sceGxmDisplayQueueAddEntry(
				mDisplayBufferSync[mFrontBufferIndex], // OLD fb
				mDisplayBufferSync[mBackBufferIndex],  // NEW fb
				&displayData);

			// update buffer indices
			mFrontBufferIndex = mBackBufferIndex;
			mBackBufferIndex = (mBackBufferIndex + 1) % GraphicsBase::DISPLAY_BUFFER_COUNT;
		}

		void GraphicsBase::startDrawing() {
			sceGxmBeginScene(
				mContext,
				0,
				mRenderTarget,
				0,
				0,
				mDisplayBufferSync[mBackBufferIndex],
				&mDisplaySurface[mBackBufferIndex],
				&mDepthSurface);
		}

		void GraphicsBase::stopDrawing() {
			sceGxmEndScene(mContext, 0, 0);
		}

	} // namespace Graphics

} // namespace NotEngine
