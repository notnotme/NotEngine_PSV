#include "../../include/notengine/graphics/Graphics2D.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"

#include <cstdlib>
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


extern const SceGxmProgram graphics2d_vert_gxp;
extern const SceGxmProgram graphics2d_frag_gxp;
static const SceGxmProgram* const s2dVertexProgramGxp	= &graphics2d_vert_gxp;
static const SceGxmProgram* const s2dFragmentProgramGxp	= &graphics2d_frag_gxp;

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
			int err = sceGxmProgramCheck(s2dVertexProgramGxp);
			if (err != 0) {
				printf("graphics2d_vert_gxp sceGxmProgramCheck(): 0x%08X\n", err);
				return false;
			}
			err = sceGxmProgramCheck(s2dFragmentProgramGxp);
			if (err != 0) {
				printf("graphics2d_frag_gxp sceGxmProgramCheck(): 0x%08X\n", err);
				return false;
			}

			// register programs with the patcher
			m2dVertexProgramId = 0;
			err = sceGxmShaderPatcherRegisterProgram(base->mShaderPatcher, s2dVertexProgramGxp, &m2dVertexProgramId);
			if (err != 0) {
				printf("graphics2d_vert_gxp sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);
				return false;
			}

			m2dFragmentProgramId = 0;
			err = sceGxmShaderPatcherRegisterProgram(base->mShaderPatcher, s2dFragmentProgramGxp, &m2dFragmentProgramId);
			if (err != 0) {
				printf("graphics2d_frag_gxp sceGxmShaderPatcherRegisterProgram(): 0x%08X\n", err);
				return false;
			}

			// get attributes by name to create vertex format bindings
			mShaderPositionAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aPosition");
			mShaderTextureAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aTexcoord");
			mShaderColorAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aColor");
			mShaderAngleAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aAngle");
			mShaderTranslationAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aTranslation");
			// gte uniforms
			mShaderMatrixProjUnif = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "pm");

			// create the 2d vertex format
			SceGxmVertexAttribute g2dVertexAttributes[5];
			// Position fmt
			g2dVertexAttributes[0] = (SceGxmVertexAttribute) {
				.streamIndex = 0,
				.offset = 0,
				.format = SCE_GXM_ATTRIBUTE_FORMAT_F32,
				.componentCount = 2,
				.regIndex = (unsigned short) sceGxmProgramParameterGetResourceIndex(mShaderPositionAttr)
			};
			// Texture fmt
			g2dVertexAttributes[1] = (SceGxmVertexAttribute) {
				.streamIndex = 0,
				.offset = (unsigned short) sizeof(float)*2,
				.format = SCE_GXM_ATTRIBUTE_FORMAT_F32,
				.componentCount = 2,
				.regIndex = (unsigned short) sceGxmProgramParameterGetResourceIndex(mShaderTextureAttr)
			};
			// Color fmt
			g2dVertexAttributes[2] = (SceGxmVertexAttribute) {
				.streamIndex = 0,
				.offset = (unsigned short)  (g2dVertexAttributes[1].offset + sizeof(float)*2),
				.format = SCE_GXM_ATTRIBUTE_FORMAT_U8N,
				.componentCount = 4,
				.regIndex = (unsigned short) sceGxmProgramParameterGetResourceIndex(mShaderColorAttr)
			};
			// Angle fmt
			g2dVertexAttributes[3] = (SceGxmVertexAttribute) {
				.streamIndex = 0,
				.offset = (unsigned short) (g2dVertexAttributes[2].offset + sizeof(unsigned char)*4),
				.format = SCE_GXM_ATTRIBUTE_FORMAT_F32,
				.componentCount = 1,
				.regIndex = (unsigned short) sceGxmProgramParameterGetResourceIndex(mShaderAngleAttr)
			};
			// Translation fmt
			g2dVertexAttributes[4] = (SceGxmVertexAttribute) {
				.streamIndex = 0,
				.offset = (unsigned short) (g2dVertexAttributes[3].offset + sizeof(float)),
				.format = SCE_GXM_ATTRIBUTE_FORMAT_F32,
				.componentCount = 2,
				.regIndex = (unsigned short) sceGxmProgramParameterGetResourceIndex(mShaderTranslationAttr)
			};
			// Indices fmt
			SceGxmVertexStream g2dVertexStreams[1] = {
				(SceGxmVertexStream) {
					.stride = sizeof(SpriteBuffer::SpriteVertice),
					.indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT
				}
			};

			// create the vertex shaders
			err = sceGxmShaderPatcherCreateVertexProgram(
				base->mShaderPatcher,
				m2dVertexProgramId,
				g2dVertexAttributes,
				5,
				g2dVertexStreams,
				1,
				&m2dVertexProgram);
			if (err != 0) {
				printf("m2dVertexProgram sceGxmShaderPatcherCreateVertexProgram(): 0x%08X\n", err);
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
				base->mShaderPatcher,
				m2dFragmentProgramId,
				SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
				(SceGxmMultisampleMode) GraphicsBase::MSAA_MODE,
				&blendInfo,
				s2dVertexProgramGxp,
				&m2dFragmentProgram);
			if (err != 0) {
				printf("m2dFragmentProgram sceGxmShaderPatcherCreateFragmentProgram(): 0x%08X\n", err);
				return false;
			}

			mBatchIndicesUID = 0;
			mBatchIndices = (unsigned short*) GraphicsBase::gpuAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE,
				MAX_SPRITES_PER_BATCH*(sizeof(unsigned short)*6),
				SCE_GXM_MEMORY_ATTRIB_READ,
				&mBatchIndicesUID);

			if (mBatchIndices == 0) {
				printf("batchIndices not allocated\n");
				return false;
			}

			// Fill the indices buffer as it will never change
			unsigned int size = MAX_SPRITES_PER_BATCH*6;
			unsigned short j = 0;
			for (unsigned int i=0; i<size; i+=6, j+=4) {
				mBatchIndices[i] = j + 3;
				mBatchIndices[i+1] = j + 2;
				mBatchIndices[i+2] = j;
				mBatchIndices[i+3] = j;
				mBatchIndices[i+4] = j + 2;
				mBatchIndices[i+5] = j + 1;
			}

			// Initialize the clear sprite and clear texture
			mClearSprite.frame.size.w = GraphicsBase::DISPLAY_WIDTH;
			mClearSprite.frame.size.h = GraphicsBase::DISPLAY_HEIGHT;
			mClearSprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
			mClearSprite.position.y = GraphicsBase::DISPLAY_HEIGHT/2;

			mClearTexture = new Texture2D();
			if (! mClearTexture->initialize(1,1, SCE_GXM_TEXTURE_FORMAT_L8)) {
				printf("clearTexture->initialize failed\n");
				return false;
			}

			// Allocate clear buffer
			mClearBuffer = new Graphics::SpriteBuffer();
			if (! mClearBuffer->initialize(32, true)) {
				printf("clearBuffer not allocated\n");
				return false;
			}

			return true;
		}

		void Graphics2D::finalize() {
			GraphicsBase* base = GraphicsBase::instance();

			if (m2dFragmentProgramId != 0) {
				sceGxmShaderPatcherReleaseFragmentProgram(base->mShaderPatcher, m2dFragmentProgram);
				sceGxmShaderPatcherUnregisterProgram(base->mShaderPatcher, m2dFragmentProgramId);
			}

			if (m2dVertexProgramId != 0) {
				sceGxmShaderPatcherReleaseVertexProgram(base->mShaderPatcher, m2dVertexProgram);
				sceGxmShaderPatcherUnregisterProgram(base->mShaderPatcher, m2dVertexProgramId);
			}

			if (mBatchIndicesUID != 0)
				GraphicsBase::gpuFree(mBatchIndicesUID);

			if (mClearTexture != 0) {
				mClearTexture->finalize();
				delete mClearTexture;
			}

			if (mClearBuffer != 0) {
				mClearBuffer->finalize();
				delete mClearBuffer;
			}
		}

		void Graphics2D::setTexture(unsigned int unit, const Graphics::Texture2D* texture) {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetFragmentTexture(base->mContext, unit, &texture->mTexture);
		}

		void Graphics2D::use() {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetVertexProgram(base->mContext, m2dVertexProgram);
			sceGxmSetFragmentProgram(base->mContext, m2dFragmentProgram);

			sceGxmSetFrontDepthWriteEnable(base->mContext, SCE_GXM_DEPTH_WRITE_DISABLED);
			sceGxmSetFrontDepthFunc(base->mContext, SCE_GXM_DEPTH_FUNC_ALWAYS);

			sceGxmReserveVertexDefaultUniformBuffer(base->mContext, &base->mVertexUniformDefaultBuffer);
		}

		void Graphics2D::render(const glm::mat4* projection, Graphics::SpriteBuffer* spriteBuffer) {
			GraphicsBase* base = GraphicsBase::instance();

			if (base->mLastBatchVerticesUID != spriteBuffer->mBatchVerticesUID) {
				base->mLastBatchVerticesUID = spriteBuffer->mBatchVerticesUID;
				sceGxmSetVertexStream(base->mContext, 0, spriteBuffer->mBatchVertices);
			}
			sceGxmSetUniformDataF(base->mVertexUniformDefaultBuffer, mShaderMatrixProjUnif, 0, 16, glm::value_ptr(*projection));

			unsigned int batchCount = spriteBuffer->mBatchCount - spriteBuffer->mBatchOffset;
			sceGxmDraw(base->mContext,
						SCE_GXM_PRIMITIVE_TRIANGLES,
						SCE_GXM_INDEX_FORMAT_U16,
						&mBatchIndices[spriteBuffer->mBatchOffset*6],
						batchCount*6);

			if (spriteBuffer->mDynamic)
				spriteBuffer->mBatchOffset += batchCount;
		}

		void Graphics2D::clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			setTexture(0, mClearTexture);
			mClearSprite.color.r = r;
			mClearSprite.color.g = g;
			mClearSprite.color.b = b;
			mClearSprite.color.a = a;

			static const glm::mat4 ortho = glm::ortho(
				0.0f, (float) Graphics::GraphicsBase::DISPLAY_WIDTH,
				(float) Graphics::GraphicsBase::DISPLAY_HEIGHT, 0.0f,
				-1.0f, 1.0f);

			mClearBuffer->start();
			mClearBuffer->put(&mClearSprite);
			render(&ortho, mClearBuffer);
		}

	} // namespace Graphics

} // namespace NotEngine
