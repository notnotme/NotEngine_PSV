#include "../../include/notengine/graphics/GraphicsBase.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace NotEngine {

	namespace Graphics {

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
			waitRetrace = enabled;
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

		void GraphicsBase::videoCallback(const void* userData) {
			const DisplayData* displayData = (const DisplayData*) userData;

			SceDisplayFrameBuf framebuf;
			memset(&framebuf, 0, sizeof(SceDisplayFrameBuf));
			framebuf.size = sizeof(SceDisplayFrameBuf);
			framebuf.base = displayData->address;
			framebuf.pitch = DISPLAY_STRIDE_IN_PIXELS;
			framebuf.pixelformat = DISPLAY_PIXEL_FORMAT;
			framebuf.width = DISPLAY_WIDTH;
			framebuf.height = DISPLAY_HEIGHT;
			sceDisplaySetFrameBuf(&framebuf, SCE_DISPLAY_SETBUF_NEXTFRAME);

			// fixme: Untested
			if (displayData->waitRetrace) {
				sceDisplayWaitVblankStart();
			}
		}

		bool GraphicsBase::initialize(bool waitForRetrace) {
			waitRetrace = waitForRetrace;

			SceGxmInitializeParams initializeParams;
			memset(&initializeParams, 0, sizeof(SceGxmInitializeParams));
			initializeParams.flags = 0;
			initializeParams.displayQueueMaxPendingCount = DISPLAY_MAX_PENDING_SWAPS;
			initializeParams.displayQueueCallback = videoCallback;
			initializeParams.displayQueueCallbackDataSize = sizeof(DisplayData);
			initializeParams.parameterBufferSize = SCE_GXM_DEFAULT_PARAMETER_BUFFER_SIZE;
			int err = sceGxmInitialize(&initializeParams);
			if (err != 0) {
				printf("sceGxmInitialize(): 0x%08X\n", err);
				return false;
			}

			// allocate ring buffer memory using default sizes
			void* vdmRingBuffer = 0;
			vdmRingBuffer = gpuAlloc(
				(SceKernelMemBlockType) SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				(unsigned int) SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE,
				SCE_GXM_MEMORY_ATTRIB_READ,
				&vdmRingBufferUid);

			void* vertexRingBuffer = 0;
			vertexRingBuffer = gpuAlloc(
				(SceKernelMemBlockType) SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				(unsigned int) SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE,
				SCE_GXM_MEMORY_ATTRIB_READ,
				&vertexRingBufferUid);

			void* fragmentRingBuffer = 0;
			fragmentRingBuffer = gpuAlloc(
				(SceKernelMemBlockType) SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				(unsigned int) SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE,
				SCE_GXM_MEMORY_ATTRIB_READ,
				&fragmentRingBufferUid);

			unsigned int fragmentUsseRingBufferOffset;
			void* fragmentUsseRingBuffer = 0;
			fragmentUsseRingBuffer = fragmentUsseAlloc(
				(unsigned int)  SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE,
				&fragmentUsseRingBufferUid,
				&fragmentUsseRingBufferOffset);

			memset(&contextParams, 0, sizeof(SceGxmContextParams));
			contextParams.hostMem = malloc(SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE);
			contextParams.hostMemSize = SCE_GXM_MINIMUM_CONTEXT_HOST_MEM_SIZE;
			contextParams.vdmRingBufferMem = vdmRingBuffer;
			contextParams.vdmRingBufferMemSize = SCE_GXM_DEFAULT_VDM_RING_BUFFER_SIZE;
			contextParams.vertexRingBufferMem = vertexRingBuffer;
			contextParams.vertexRingBufferMemSize = SCE_GXM_DEFAULT_VERTEX_RING_BUFFER_SIZE;
			contextParams.fragmentRingBufferMem = fragmentRingBuffer;
			contextParams.fragmentRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_RING_BUFFER_SIZE;
			contextParams.fragmentUsseRingBufferMem = fragmentUsseRingBuffer;
			contextParams.fragmentUsseRingBufferMemSize = SCE_GXM_DEFAULT_FRAGMENT_USSE_RING_BUFFER_SIZE;
			contextParams.fragmentUsseRingBufferOffset = fragmentUsseRingBufferOffset;
			context = 0;

			err = sceGxmCreateContext(&contextParams, &context);
			if(err != 0) {
				printf("sceGxmCreateContext(): 0x%08X\n", err);
				return false;
			}

			// create the render target
			SceGxmRenderTargetParams renderTargetParams;
			memset(&renderTargetParams, 0, sizeof(SceGxmRenderTargetParams));
			renderTargetParams.flags = 0;
			renderTargetParams.width = DISPLAY_WIDTH;
			renderTargetParams.height = DISPLAY_HEIGHT;
			renderTargetParams.scenesPerFrame = 1;
			renderTargetParams.multisampleMode = MSAA_MODE;
			renderTargetParams.multisampleLocations = 0;
			renderTargetParams.driverMemBlock = -1; // Invalid UID
			err = sceGxmCreateRenderTarget(&renderTargetParams, &renderTarget);
			if (err != 0) {
	 			printf("sceGxmCreateRenderTarget(): 0x%08X\n", err);
	 			return false;
			}

			// allocate memory and sync objects for display buffers
			for (unsigned int i=0; i<DISPLAY_BUFFER_COUNT; i++) {
				// initialize a color surface for this display buffer
				displayBufferData[i] = 0;
				displayBufferData[i] = gpuAlloc(
					SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
					4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT,
					SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
					&displayBufferUid[i]);

				memset(displayBufferData[i], 0xff000000, 4*DISPLAY_STRIDE_IN_PIXELS*DISPLAY_HEIGHT);
				err = sceGxmColorSurfaceInit(
					&displaySurface[i],
					(SceGxmColorFormat) DISPLAY_COLOR_FORMAT,
					SCE_GXM_COLOR_SURFACE_LINEAR,
					(MSAA_MODE == SCE_GXM_MULTISAMPLE_NONE) ? SCE_GXM_COLOR_SURFACE_SCALE_NONE : SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE,
					SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
					DISPLAY_WIDTH,
					DISPLAY_HEIGHT,
					DISPLAY_STRIDE_IN_PIXELS,
					displayBufferData[i]);
				if (err != 0) {
		 			printf("sceGxmColorSurfaceInit(): 0x%08X\n", err);
		 			return false;
		 		}

				// create a sync object that we will associate with this buffer
				err = sceGxmSyncObjectCreate(&displayBufferSync[i]);
				if (err != 0) {
		 			printf("sceGxmSyncObjectCreate(): 0x%08X\n", err);
		 			return false;
				}
			}

			// compute the memory footprint of the depth buffer
			const unsigned int alignedWidth = align(DISPLAY_WIDTH, SCE_GXM_TILE_SIZEX);
			const unsigned int alignedHeight = align(DISPLAY_HEIGHT, SCE_GXM_TILE_SIZEY);
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
			depthBufferData = gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				4*sampleCount,
				SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
				&depthBufferUid);

			err = sceGxmDepthStencilSurfaceInit(
				&depthSurface,
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
			patcherBuffer = gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				patcherBufferSize,
				SCE_GXM_MEMORY_ATTRIB_READ | SCE_GXM_MEMORY_ATTRIB_WRITE,
				&patcherBufferUid);

			unsigned int patcherVertexUsseOffset;
			void* patcherVertexUsse = 0;
			patcherVertexUsse = vertexUsseAlloc(
				patcherVertexUsseSize,
				&patcherVertexUsseUid,
				&patcherVertexUsseOffset);

			unsigned int patcherFragmentUsseOffset;
			void* patcherFragmentUsse = 0;
			patcherFragmentUsse = fragmentUsseAlloc(
				patcherFragmentUsseSize,
				&patcherFragmentUsseUid,
				&patcherFragmentUsseOffset);

			// create a shader patcher
			SceGxmShaderPatcherParams patcherParams;
			memset(&patcherParams, 0, sizeof(SceGxmShaderPatcherParams));
			patcherParams.userData = 0;
			patcherParams.hostAllocCallback = &patcherAlloc;
			patcherParams.hostFreeCallback = &patcherFree;
			patcherParams.bufferAllocCallback = 0;
			patcherParams.bufferFreeCallback = 0;
			patcherParams.bufferMem = patcherBuffer;
			patcherParams.bufferMemSize = patcherBufferSize;
			patcherParams.vertexUsseAllocCallback = 0;
			patcherParams.vertexUsseFreeCallback = 0;
			patcherParams.vertexUsseMem	 = patcherVertexUsse;
			patcherParams.vertexUsseMemSize = patcherVertexUsseSize;
			patcherParams.vertexUsseOffset = patcherVertexUsseOffset;
			patcherParams.fragmentUsseAllocCallback = 0;
			patcherParams.fragmentUsseFreeCallback = 0;
			patcherParams.fragmentUsseMem = patcherFragmentUsse;
			patcherParams.fragmentUsseMemSize = patcherFragmentUsseSize;
			patcherParams.fragmentUsseOffset = patcherFragmentUsseOffset;

			err = sceGxmShaderPatcherCreate(&patcherParams, &shaderPatcher);
			if (err != 0) {
				printf("sceGxmShaderPatcherCreate(): 0x%08X\n", err);
				return false;
			}

			sceGxmSetCullMode(context, SCE_GXM_CULL_CW);

			backBufferIndex = 0;
			frontBufferIndex = 0;

			return true;
		}

		void GraphicsBase::waitTerminate() {
			// wait until rendering is done
			if (context != 0) {
				sceGxmFinish(context);
				sceGxmDisplayQueueFinish();
			}
		}

		void GraphicsBase::finalize() {
			// clean up display queue
			if (depthBufferUid != 0)
				gpuFree(depthBufferUid);

			for (unsigned int i=0; i<DISPLAY_BUFFER_COUNT; i++) {
				// clear the buffer then deallocate
				if (displayBufferUid[i] != 0) {
					memset(displayBufferData[i], 0, DISPLAY_HEIGHT*DISPLAY_STRIDE_IN_PIXELS*4);
					gpuFree(displayBufferUid[i]);
					sceGxmSyncObjectDestroy(displayBufferSync[i]);
				}
			}

			if (renderTarget != 0)
				sceGxmDestroyRenderTarget(renderTarget);

			if (shaderPatcher != 0)
				sceGxmShaderPatcherDestroy(shaderPatcher);

			if (patcherFragmentUsseUid != 0)
				fragmentUsseFree(patcherFragmentUsseUid);

			if (patcherVertexUsseUid != 0)
				vertexUsseFree(patcherVertexUsseUid);

			if (patcherBufferUid != 0)
				gpuFree(patcherBufferUid);

			if (context != 0)
				sceGxmDestroyContext(context);

			if (fragmentUsseRingBufferUid != 0)
				fragmentUsseFree(fragmentUsseRingBufferUid);

			if (fragmentRingBufferUid != 0)
				gpuFree(fragmentRingBufferUid);

			if (vertexRingBufferUid != 0)
				gpuFree(vertexRingBufferUid);

			if (vdmRingBufferUid != 0)
				gpuFree(vdmRingBufferUid);

			if (contextParams.hostMem != 0)
				free(contextParams.hostMem);

			// terminate libgxm
			sceGxmTerminate();
		}

		void GraphicsBase::swapBuffers() {
			sceGxmPadHeartbeat(&displaySurface[backBufferIndex], displayBufferSync[backBufferIndex]);

			// queue the display swap for this frame
			DisplayData displayData;
			displayData.address = displayBufferData[backBufferIndex];
			displayData.waitRetrace = waitRetrace;

			sceGxmDisplayQueueAddEntry(
				displayBufferSync[frontBufferIndex], // OLD fb
				displayBufferSync[backBufferIndex],  // NEW fb
				&displayData);

			// update buffer indices
			frontBufferIndex = backBufferIndex;
			backBufferIndex = (backBufferIndex + 1) % DISPLAY_BUFFER_COUNT;
		}

		void GraphicsBase::startDrawing() {
			sceGxmBeginScene(
				context,
				0,
				renderTarget,
				0,
				0,
				displayBufferSync[backBufferIndex],
				&displaySurface[backBufferIndex],
				&depthSurface);
		}

		void GraphicsBase::stopDrawing() {
			sceGxmEndScene(context, 0, 0);
		}

	} // namespace Graphics

} // namespace NotEngine
