#include "Graphics2D.hpp"

#include <stdlib.h>
#include <stdio.h>
#include "GraphicsBase.hpp"
#include <glm/gtc/type_ptr.hpp>

extern const SceGxmProgram graphics2d_vert_gxp;
extern const SceGxmProgram graphics2d_frag_gxp;
static const SceGxmProgram* const g2dVertexProgramGxp     = &graphics2d_vert_gxp;
static const SceGxmProgram* const g2dFragmentProgramGxp   = &graphics2d_frag_gxp;

namespace NotEngine {

	namespace Graphics {

		Graphics2D::Graphics2D() : System::Singleton<Graphics2D>() {
		}

		Graphics2D::~Graphics2D() {
		}

		bool Graphics2D::initialize(unsigned int batchSize) {
			GraphicsBase* base = GraphicsBase::instance();
			batchCapacity = batchSize;
			batchCount = 0;

			// Check if the 2d shaders are valids
			int err = sceGxmProgramCheck(g2dVertexProgramGxp);
			if (err != 0) {
				printf("graphics2d_vert_gxp sceGxmProgramCheck(): 0x%08X", err);
				return false;
			}
			err = sceGxmProgramCheck(g2dFragmentProgramGxp);
			if (err != 0) {
				printf("graphics2d_frag_gxp sceGxmProgramCheck(): 0x%08X", err);
				return false;
			}

			// register programs with the patcher
			err = sceGxmShaderPatcherRegisterProgram(base->shaderPatcher, g2dVertexProgramGxp, &g2dVertexProgramId);
			if (err != 0) {
				printf("graphics2d_vert_gxp sceGxmShaderPatcherRegisterProgram(): 0x%08X", err);
				return false;
			}
			err = sceGxmShaderPatcherRegisterProgram(base->shaderPatcher, g2dFragmentProgramGxp, &g2dFragmentProgramId);
			if (err != 0) {
				printf("graphics2d_frag_gxp sceGxmShaderPatcherRegisterProgram(): 0x%08X", err);
				return false;
			}

			// get attributes by name to create vertex format bindings
			shaderPositionAttr = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "aPosition");
			shaderTextureAttr = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "aTexcoord");
			shaderColorAttr = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "aColor");
			// gte uniforms
			shaderMatrixProjUnif = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "pm");

			// create the 2d vertex format
			SceGxmVertexAttribute g2dVertexAttributes[3];
			// Position fmt
			g2dVertexAttributes[0].streamIndex = 0;
			g2dVertexAttributes[0].offset = 0;
			g2dVertexAttributes[0].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			g2dVertexAttributes[0].componentCount = 2;
			g2dVertexAttributes[0].regIndex = sceGxmProgramParameterGetResourceIndex(shaderPositionAttr);
			// Texture fmt
			g2dVertexAttributes[1].streamIndex = 0;
			g2dVertexAttributes[1].offset = sizeof(float)*2;
			g2dVertexAttributes[1].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			g2dVertexAttributes[1].componentCount = 2;
			g2dVertexAttributes[1].regIndex = sceGxmProgramParameterGetResourceIndex(shaderTextureAttr);
			// Color fmt
			g2dVertexAttributes[2].streamIndex = 0;
			g2dVertexAttributes[2].offset = g2dVertexAttributes[1].offset + sizeof(float)*2;
			g2dVertexAttributes[2].format = SCE_GXM_ATTRIBUTE_FORMAT_U8N;
			g2dVertexAttributes[2].componentCount = 4;
			g2dVertexAttributes[2].regIndex = sceGxmProgramParameterGetResourceIndex(shaderColorAttr);
			// Indices fmt
			SceGxmVertexStream g2dVertexStreams[1];
			g2dVertexStreams[0].stride = sizeof(SpriteVertice);
			g2dVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

			// create the vertex shaders
			err = sceGxmShaderPatcherCreateVertexProgram(
				base->shaderPatcher,
				g2dVertexProgramId,
				g2dVertexAttributes,
				3,
				g2dVertexStreams,
				1,
				&g2dVertexProgram);
			if (err != 0) {
				printf("g2dVertexProgram sceGxmShaderPatcherCreateVertexProgram(): 0x%08X", err);
				return false;
			}

			// Then the fragment shader
			SceGxmBlendInfo blendInfo = {
				.colorMask = SCE_GXM_COLOR_MASK_ALL,
				.colorFunc = SCE_GXM_BLEND_FUNC_ADD,
				.alphaFunc = SCE_GXM_BLEND_FUNC_ADD,
				.colorSrc  = SCE_GXM_BLEND_FACTOR_SRC_ALPHA,
				.colorDst  = SCE_GXM_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				.alphaSrc  = SCE_GXM_BLEND_FACTOR_ONE,
				.alphaDst  = SCE_GXM_BLEND_FACTOR_ZERO,
			};
			err = sceGxmShaderPatcherCreateFragmentProgram(
				base->shaderPatcher,
				g2dFragmentProgramId,
				SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
				(SceGxmMultisampleMode) GraphicsBase::MSAA_MODE,
				&blendInfo,
				g2dVertexProgramGxp,
				&g2dFragmentProgram);
			if (err != 0) {
				printf("g2dFragmentProgram sceGxmShaderPatcherCreateFragmentProgram(): 0x%08X", err);
				return false;
			}

			// Allocate buffers
			batchVertices = (SpriteVertice*) GraphicsBase::gpuAlloc(
												SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
												batchCapacity*(sizeof(SpriteVertice)*4),
												SCE_GXM_MEMORY_ATTRIB_READ,
												&batchVerticesUID);

			batchIndices = (unsigned short*) GraphicsBase::gpuAlloc(
												SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
												batchCapacity*(sizeof(unsigned short)*6),
												SCE_GXM_MEMORY_ATTRIB_READ,
												&batchIndicesUID);

			// Fill the indices buffer as it will never change
			// Pre render the indices as they never change
			unsigned int size = batchCapacity*6;
			unsigned short j = 0;
			for (unsigned int i=0; i<size; i+=6, j+=4) {
				batchIndices[i] = j + 3;
				batchIndices[i+1] = j + 2;
				batchIndices[i+2] = j;
				batchIndices[i+3] = j;
				batchIndices[i+4] = j + 2;
				batchIndices[i+5] = j + 1;
			}

			// Initialize the clear sprite and clear texture
			clearSprite.size.w = GraphicsBase::DISPLAY_WIDTH;
			clearSprite.size.h = GraphicsBase::DISPLAY_HEIGHT;
			clearSprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
			clearSprite.position.y = GraphicsBase::DISPLAY_HEIGHT/2;
			clearTexture = new Texture2D();
			if (! clearTexture->initialize(1,1, SCE_GXM_TEXTURE_FORMAT_L8)) {
				printf("%s", "clearTexture->initialize failed");
				return false;
			}

			return true;
		}

		void Graphics2D::finalize() {
			GraphicsBase* base = GraphicsBase::instance();

			sceGxmShaderPatcherReleaseFragmentProgram(base->shaderPatcher, g2dFragmentProgram);
			sceGxmShaderPatcherReleaseVertexProgram(base->shaderPatcher, g2dVertexProgram);
			sceGxmShaderPatcherUnregisterProgram(base->shaderPatcher, g2dFragmentProgramId);
			sceGxmShaderPatcherUnregisterProgram(base->shaderPatcher, g2dVertexProgramId);

			clearTexture->finalize();
			delete clearTexture;

			GraphicsBase::gpuFree(batchIndicesUID);
			GraphicsBase::gpuFree(batchVerticesUID);
		}

		void Graphics2D::setTexture(unsigned int unit, const Graphics::Texture2D* texture) {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetFragmentTexture(base->context, unit, &texture->texture);
		}

		void Graphics2D::use(const glm::mat4* projection) {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetVertexProgram(base->context, g2dVertexProgram);
			sceGxmSetFragmentProgram(base->context, g2dFragmentProgram);
			sceGxmSetVertexStream(base->context, 0, batchVertices);

			void *vertexDefaultBuffer;
			sceGxmReserveVertexDefaultUniformBuffer(base->context, &vertexDefaultBuffer);
			sceGxmSetUniformDataF(vertexDefaultBuffer, shaderMatrixProjUnif, 0, 16, glm::value_ptr(*projection));

			// This may disable Z write ? How we cull the face ?
			sceGxmSetBackDepthWriteEnable(base->context, SCE_GXM_DEPTH_WRITE_DISABLED);
			sceGxmSetFrontDepthWriteEnable(base->context, SCE_GXM_DEPTH_WRITE_DISABLED);
		}

		void Graphics2D::unuse() {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetBackDepthWriteEnable(base->context, SCE_GXM_DEPTH_WRITE_ENABLED);
			sceGxmSetFrontDepthWriteEnable(base->context, SCE_GXM_DEPTH_WRITE_ENABLED);
		}

		void Graphics2D::resetBatch() {
			batchOffset = 0;
			batchCount = 0;
		}

		void Graphics2D::startBatch() {
			batchCount = 0;
		}

		void Graphics2D::addToBatch(const Graphics::Sprite* sprite) {
			if (batchCount >= batchCapacity) {
				printf("%s", "addToBatch discard. Capacity overflow");
				return;
			}

			// todo scale & rotation
			unsigned int index = (batchOffset+batchCount)*4;
			float wdth = sprite->size.w/2;
			float hght = sprite->size.h/2;

			batchVertices[index].x = sprite->position.x - wdth * sprite->scale.w;
			batchVertices[index].y = sprite->position.y - hght * sprite->scale.h;
			batchVertices[index].s = sprite->frame.s;
			batchVertices[index].t = sprite->frame.t;
			batchVertices[index].color = sprite->color;
			//batchVertices[index].angle = angle;
			//batchVertices[index].px = sprite->position.x;
			//batchVertices[index].py = sprite->position.y;
			index++;

			batchVertices[index].x = sprite->position.x + wdth * sprite->scale.w;
			batchVertices[index].y = sprite->position.y - hght * sprite->scale.h;
			batchVertices[index].s = sprite->frame.u;
			batchVertices[index].t = sprite->frame.t;
			batchVertices[index].color = sprite->color;
			//batchVertices[index].angle = angle;
			//batchVertices[index].px = sprite->position.x;
			//batchVertices[index].py = sprite->position.y;
			index++;

			batchVertices[index].x = sprite->position.x + wdth * sprite->scale.w;
			batchVertices[index].y = sprite->position.y + hght * sprite->scale.h;
			batchVertices[index].s = sprite->frame.u;
			batchVertices[index].t = sprite->frame.v;
			batchVertices[index].color = sprite->color;
			//batchVertices[index].angle = angle;
			//batchVertices[index].px = sprite->position.x;
			//batchVertices[index].py = sprite->position.y;
			index++;

			batchVertices[index].x = sprite->position.x - wdth * sprite->scale.w;
			batchVertices[index].y = sprite->position.y + hght * sprite->scale.h;
			batchVertices[index].s = sprite->frame.s;
			batchVertices[index].t = sprite->frame.v;
			batchVertices[index].color = sprite->color;
			//batchVertices[index].angle = angle;
			//batchVertices[index].px = sprite->position.x;
			//batchVertices[index].py = sprite->position.y;

			batchCount++;
		}

		void Graphics2D::renderBatch() {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmDraw(base->context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, &batchIndices[batchOffset*6], batchCount*6);
			batchOffset += batchCount;
		}

		void Graphics2D::clear(unsigned int color) {
			clearSprite.color = color;
			setTexture(0, clearTexture);
			startBatch();
			addToBatch(&clearSprite);
			renderBatch();
		}

	} // namespace Graphics

} // namespace NotEngine
