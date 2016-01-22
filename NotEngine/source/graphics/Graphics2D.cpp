#include "../../include/notengine/graphics/Graphics2D.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


extern const SceGxmProgram graphics2d_vert_gxp;
extern const SceGxmProgram graphics2d_frag_gxp;
static const SceGxmProgram* const g2dVertexProgramGxp     = &graphics2d_vert_gxp;
static const SceGxmProgram* const g2dFragmentProgramGxp   = &graphics2d_frag_gxp;

namespace NotEngine {

	namespace Graphics {

		Graphics2D::Graphics2D() : System::Singleton<Graphics2D>() {
			printf("Graphics2D()\n");
		}

		Graphics2D::~Graphics2D() {
			printf("~Graphics2D()\n");
		}

		bool Graphics2D::initialize() {
			GraphicsBase* base = GraphicsBase::instance();

			// Check if the 2d shaders are valids
			int err = sceGxmProgramCheck(g2dVertexProgramGxp);
			if (err != 0) {
				printf("graphics2d_vert_gxp sceGxmProgramCheck(): 0x%08X\n", err);
				return false;
			}
			err = sceGxmProgramCheck(g2dFragmentProgramGxp);
			if (err != 0) {
				printf("graphics2d_frag_gxp sceGxmProgramCheck(): 0x%08X\n", err);
				return false;
			}

			// register programs with the patcher
			g2dVertexProgramId = 0;
			err = sceGxmShaderPatcherRegisterProgram(base->shaderPatcher, g2dVertexProgramGxp, &g2dVertexProgramId);
			if (err != 0) {
				printf("graphics2d_vert_gxp sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);
				return false;
			}

			g2dFragmentProgramId = 0;
			err = sceGxmShaderPatcherRegisterProgram(base->shaderPatcher, g2dFragmentProgramGxp, &g2dFragmentProgramId);
			if (err != 0) {
				printf("graphics2d_frag_gxp sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);
				return false;
			}

			// get attributes by name to create vertex format bindings
			shaderPositionAttr = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "aPosition");
			shaderTextureAttr = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "aTexcoord");
			shaderColorAttr = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "aColor");
			shaderAngleAttr = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "aAngle");
			shaderTranslationAttr = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "aTranslation");
			// gte uniforms
			shaderMatrixProjUnif = sceGxmProgramFindParameterByName(g2dVertexProgramGxp, "pm");

			// create the 2d vertex format
			SceGxmVertexAttribute g2dVertexAttributes[5];
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
			// Angle fmt
			g2dVertexAttributes[3].streamIndex = 0;
			g2dVertexAttributes[3].offset = g2dVertexAttributes[2].offset + sizeof(unsigned char)*4;
			g2dVertexAttributes[3].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			g2dVertexAttributes[3].componentCount = 1;
			g2dVertexAttributes[3].regIndex = sceGxmProgramParameterGetResourceIndex(shaderAngleAttr);
			// Translation fmt
			g2dVertexAttributes[4].streamIndex = 0;
			g2dVertexAttributes[4].offset = g2dVertexAttributes[3].offset + sizeof(float);
			g2dVertexAttributes[4].format = SCE_GXM_ATTRIBUTE_FORMAT_F32;
			g2dVertexAttributes[4].componentCount = 2;
			g2dVertexAttributes[4].regIndex = sceGxmProgramParameterGetResourceIndex(shaderTranslationAttr);
			// Indices fmt
			SceGxmVertexStream g2dVertexStreams[1];
			g2dVertexStreams[0].stride = sizeof(SpriteBuffer::SpriteVertice);
			g2dVertexStreams[0].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT;

			// create the vertex shaders
			err = sceGxmShaderPatcherCreateVertexProgram(
				base->shaderPatcher,
				g2dVertexProgramId,
				g2dVertexAttributes,
				5,
				g2dVertexStreams,
				1,
				&g2dVertexProgram);
			if (err != 0) {
				printf("g2dVertexProgram sceGxmShaderPatcherCreateVertexProgram(): 0x%08X\n", err);
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
				printf("g2dFragmentProgram sceGxmShaderPatcherCreateFragmentProgram(): 0x%08X\n", err);
				return false;
			}

			batchIndicesUID = 0;
			batchIndices = (unsigned short*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				MAX_SPRITES_PER_BATCH*(sizeof(unsigned short)*6),
				SCE_GXM_MEMORY_ATTRIB_READ,
				&batchIndicesUID);

			if (batchIndices == 0) {
				printf("batchIndices not allocated\n");
				return false;
			}

			// Fill the indices buffer as it will never change
			unsigned int size = MAX_SPRITES_PER_BATCH*6;
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
				printf("clearTexture->initialize failed\n");
				return false;
			}

			// Allocate clear buffer
			clearBuffer = new Graphics::SpriteBuffer();
			if (! clearBuffer->initialize(1)) {
				printf("clearBuffer not allocated\n");
				return false;
			}
			clearBuffer->start();
			clearBuffer->put(&clearSprite);

			return true;
		}

		void Graphics2D::finalize() {
			GraphicsBase* base = GraphicsBase::instance();

			if (g2dFragmentProgramId != 0) {
				sceGxmShaderPatcherReleaseFragmentProgram(base->shaderPatcher, g2dFragmentProgram);
				sceGxmShaderPatcherUnregisterProgram(base->shaderPatcher, g2dFragmentProgramId);
			}

			if (g2dVertexProgramId != 0) {
				sceGxmShaderPatcherReleaseVertexProgram(base->shaderPatcher, g2dVertexProgram);
				sceGxmShaderPatcherUnregisterProgram(base->shaderPatcher, g2dVertexProgramId);
			}

			if (batchIndicesUID != 0)
				GraphicsBase::gpuFree(batchIndicesUID);

			if (clearTexture != 0) {
				clearTexture->finalize();
				delete clearTexture;
			}

			if (clearBuffer != 0) {
				clearBuffer->finalize();
				delete clearBuffer;
			}
		}

		void Graphics2D::setTexture(unsigned int unit, const Graphics::Texture2D* texture) {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetFragmentTexture(base->context, unit, &texture->texture);
		}

		void Graphics2D::use() {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetVertexProgram(base->context, g2dVertexProgram);
			sceGxmSetFragmentProgram(base->context, g2dFragmentProgram);
			sceGxmSetBackDepthWriteEnable(base->context, SCE_GXM_DEPTH_WRITE_DISABLED);
			sceGxmSetFrontDepthWriteEnable(base->context, SCE_GXM_DEPTH_WRITE_DISABLED);
		}

		void Graphics2D::unuse() {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetBackDepthWriteEnable(base->context, SCE_GXM_DEPTH_WRITE_ENABLED);
			sceGxmSetFrontDepthWriteEnable(base->context, SCE_GXM_DEPTH_WRITE_ENABLED);
		}

		void Graphics2D::render(const glm::mat4* projection, const Graphics::SpriteBuffer* spriteBuffer) {
			if (spriteBuffer->batchCount >= MAX_SPRITES_PER_BATCH) {
				printf("Graphics2D Can't render SpriteBuffer because it is too big\n");
				return;
			}

			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetVertexStream(base->context, 0, spriteBuffer->batchVertices);

			void *vertexDefaultBuffer;
			sceGxmReserveVertexDefaultUniformBuffer(base->context, &vertexDefaultBuffer);
			sceGxmSetUniformDataF(vertexDefaultBuffer, shaderMatrixProjUnif, 0, 16, glm::value_ptr(*projection));

			sceGxmDraw(base->context, SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, &batchIndices[0], spriteBuffer->batchCount*6);
		}

		void Graphics2D::clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			setTexture(0, clearTexture);

			clearSprite.color = (Sprite::SpriteColor) {r,g,b,a};

			glm::mat4 ortho = glm::ortho(
				0.0f, (float) Graphics::GraphicsBase::DISPLAY_WIDTH,
				(float) Graphics::GraphicsBase::DISPLAY_HEIGHT, 0.0f,
				-1.0f, 1.0f);

			clearBuffer->start();
			clearBuffer->put(&clearSprite);
			render(&ortho, clearBuffer);
		}

	} // namespace Graphics

} // namespace NotEngine
