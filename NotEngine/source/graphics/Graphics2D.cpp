#include "../../include/notengine/graphics/Graphics2D.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern const SceGxmProgram graphics2d_vert_gxp;
extern const SceGxmProgram graphics2d_frag_gxp;
static const SceGxmProgram* const s2dVertexProgramGxp	= &graphics2d_vert_gxp;
static const SceGxmProgram* const s2dFragmentProgramGxp	= &graphics2d_frag_gxp;

namespace NotEngine {

	namespace Graphics {

		Graphics2D::Graphics2D() : System::Singleton<Graphics2D>() {
		}

		Graphics2D::~Graphics2D() {
		}

		int Graphics2D::initialize() {
			GraphicsBase* base = GraphicsBase::instance();

			// Check if the 2d shaders are valids
			int err = sceGxmProgramCheck(s2dVertexProgramGxp);
			if (err != 0) {
				return VERTEX_SCEGXM_PROGRAM_CHECK;
			}
			err = sceGxmProgramCheck(s2dFragmentProgramGxp);
			if (err != 0) {
				return FRAGMENT_SCEGXM_PROGRAM_CHECK;
			}

			// register programs with the patcher
			m2dVertexProgramId = 0;
			err = sceGxmShaderPatcherRegisterProgram(base->mShaderPatcher, s2dVertexProgramGxp, &m2dVertexProgramId);
			if (err != 0) {
				return VERTEX_SCEGXM_REGISTER_PROGRAM;
			}

			m2dFragmentProgramId = 0;
			err = sceGxmShaderPatcherRegisterProgram(base->mShaderPatcher, s2dFragmentProgramGxp, &m2dFragmentProgramId);
			if (err != 0) {
				return FRAGMENT_SCEGXM_REGISTER_PROGRAM;
			}

			// get attributes by name to create vertex format bindings
			mShaderPositionAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aPosition");
			mShaderTextureAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aTexcoord");
			mShaderColorAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aColor");
			mShaderAngleAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aAngle");
			mShaderTranslationAttr = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "aTranslation");
			// get uniforms
			mShaderMatrixProjUnif = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "pm");
			mShaderTextureEnableUnif = sceGxmProgramFindParameterByName(s2dFragmentProgramGxp, "textureEnable");
			mShaderTRSEnableUnif = sceGxmProgramFindParameterByName(s2dVertexProgramGxp, "trsEnable");


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
				return VERTEX_SCEGXM_CREATE_PROGRAM;
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
				return FRAGMENT_SCEGXM_CREATE_PROGRAM;
			}

			mSpriteIndiceBuffer = new IndiceBuffer();
			err = mSpriteIndiceBuffer->initialize(MAX_VERTICES_PER_BATCH);
			if (err != IndiceBuffer::NO_ERROR) {
				return SPRITES_INDICES_GPU_ALLOC;
			}

			// Fill the sprite indices buffer as it will never change
			unsigned int size = MAX_VERTICES_PER_BATCH;
			unsigned short j = 0;
			mSpriteIndiceBuffer->start();
			for (unsigned int i=0; i<size; i+=6, j+=4) {
				mSpriteIndiceBuffer->put(j + 3);
				mSpriteIndiceBuffer->put(j + 2);
				mSpriteIndiceBuffer->put(j);
				mSpriteIndiceBuffer->put(j);
				mSpriteIndiceBuffer->put(j + 2);
				mSpriteIndiceBuffer->put(j + 1);
			}

			mShapeIndiceBuffer = new IndiceBuffer();
			err = mShapeIndiceBuffer->initialize(MAX_VERTICES_PER_BATCH);
			if (err != IndiceBuffer::NO_ERROR) {
				return SHAPES_INDICES_GPU_ALLOC;
			}

			// Fill the shape indices buffer as it will never change
			mShapeIndiceBuffer->start();
			for (unsigned int i=0; i<size; i++) {
				mShapeIndiceBuffer->put(i);
			}

			// Initialize the clear sprite and clear texture
			mClearSprite.frame.size.w = GraphicsBase::DISPLAY_WIDTH;
			mClearSprite.frame.size.h = GraphicsBase::DISPLAY_HEIGHT;
			mClearSprite.position.x = GraphicsBase::DISPLAY_WIDTH/2;
			mClearSprite.position.y = GraphicsBase::DISPLAY_HEIGHT/2;

			mClearTexture = new Texture2D();
			if (mClearTexture->initialize(1,1, SCE_GXM_TEXTURE_FORMAT_L8) != Texture2D::NO_ERROR) {
				return CLEAR_TEXTURE_INITIALIZE;
			}

			// Allocate clear buffer
			mClearBuffer = new SpriteBuffer();
			if (mClearBuffer->initialize(32, true) != SpriteBuffer::NO_ERROR) {
				return CLEAR_BUFFER_INITIALIZE;
			}

			return NO_ERROR;
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

			if (mSpriteIndiceBuffer != 0) {
				mSpriteIndiceBuffer->finalize();
				delete mSpriteIndiceBuffer;
			}

			if (mShapeIndiceBuffer != 0) {
				mShapeIndiceBuffer->finalize();
				delete mShapeIndiceBuffer;
			}

			if (mClearTexture != 0) {
				mClearTexture->finalize();
				delete mClearTexture;
			}

			if (mClearBuffer != 0) {
				mClearBuffer->finalize();
				delete mClearBuffer;
			}
		}

		void Graphics2D::setTexture(const Texture2D* texture) const {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetFragmentTexture(base->mContext, 0, &texture->mTexture);
		}

		void Graphics2D::setProjectionMatrix(const glm::mat4& projection) const {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetUniformDataF(base->mVertexUniformDefaultBuffer, mShaderMatrixProjUnif, 0, 16, glm::value_ptr(projection));
		}

		void Graphics2D::use() const {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetVertexProgram(base->mContext, m2dVertexProgram);
			sceGxmSetFragmentProgram(base->mContext, m2dFragmentProgram);

			sceGxmSetFrontDepthWriteEnable(base->mContext, SCE_GXM_DEPTH_WRITE_DISABLED);
			sceGxmSetFrontDepthFunc(base->mContext, SCE_GXM_DEPTH_FUNC_ALWAYS);

			sceGxmReserveFragmentDefaultUniformBuffer(base->mContext, &base->mFragmentUniformDefaultBuffer);
			sceGxmReserveVertexDefaultUniformBuffer(base->mContext, &base->mVertexUniformDefaultBuffer);
		}

		void Graphics2D::render(SpriteBuffer* spriteBuffer) const {
			GraphicsBase* base = GraphicsBase::instance();

			if (base->mLastBatchVerticesUID != spriteBuffer->mBatchVerticesUID) {
				base->mLastBatchVerticesUID = spriteBuffer->mBatchVerticesUID;
				sceGxmSetVertexStream(base->mContext, 0, spriteBuffer->mBatchVertices);
			}

			float textureEnable = 1.0f;
			float trsEnable = 1.0f;
			sceGxmSetUniformDataF(base->mFragmentUniformDefaultBuffer, mShaderTextureEnableUnif, 0, 1, &textureEnable);
			sceGxmSetUniformDataF(base->mVertexUniformDefaultBuffer, mShaderTRSEnableUnif, 0, 1, &trsEnable);

			unsigned int batchCount = spriteBuffer->mBatchCount - spriteBuffer->mBatchOffset;
			sceGxmDraw(base->mContext,
						SCE_GXM_PRIMITIVE_TRIANGLES,
						SCE_GXM_INDEX_FORMAT_U16,
						&mSpriteIndiceBuffer->mIndices[spriteBuffer->mBatchOffset*6],
						batchCount*6);

			if (spriteBuffer->mDynamic)
				spriteBuffer->mBatchOffset += batchCount;
		}

		int Graphics2D::render(SceGxmPrimitiveType type, D2Buffer* vertices, bool texture) const {
			return render(type, mShapeIndiceBuffer, vertices, texture, vertices->mVerticesOffset, vertices->mVerticesCount - vertices->mVerticesOffset);
		}

		int Graphics2D::render(SceGxmPrimitiveType type, IndiceBuffer* indices, D2Buffer* vertices, bool texture, int startIndice, int indiceCount) const {
			switch(type) {
				// chek for errors
				case SCE_GXM_PRIMITIVE_TRIANGLES:
					if(indiceCount % 3 != 0 || indiceCount < 3) {
						return WRONG_VERTICES_COUNT;
					}
					break;
				case SCE_GXM_PRIMITIVE_TRIANGLE_EDGES: // not sure about this one
				case SCE_GXM_PRIMITIVE_TRIANGLE_FAN:
				case SCE_GXM_PRIMITIVE_TRIANGLE_STRIP:
					if(indiceCount < 3) {
						return WRONG_VERTICES_COUNT;
					}
					break;
				case SCE_GXM_PRIMITIVE_LINES:
					if(indiceCount % 2 != 0 || indiceCount < 2) {
						return WRONG_VERTICES_COUNT;
					}
					break;
				case SCE_GXM_PRIMITIVE_POINTS:
					if(indiceCount < 1) {
						return WRONG_VERTICES_COUNT;
					}
					break;
			}

			GraphicsBase* base = GraphicsBase::instance();

			if (base->mLastBatchVerticesUID != vertices->mBatchVerticesUID) {
				base->mLastBatchVerticesUID = vertices->mBatchVerticesUID;
				sceGxmSetVertexStream(base->mContext, 0, vertices->mBatchVertices);
			}

			float textureEnable = 0.0f;
			if (texture) {
				textureEnable = 1.0f;
			}
			float trsEnable = 0.0f;

			sceGxmSetUniformDataF(base->mFragmentUniformDefaultBuffer, mShaderTextureEnableUnif, 0, 1, &textureEnable);
			sceGxmSetUniformDataF(base->mVertexUniformDefaultBuffer, mShaderTRSEnableUnif, 0, 1, &trsEnable);

			sceGxmDraw(base->mContext,
						type,
						SCE_GXM_INDEX_FORMAT_U16,
						&indices->mIndices[startIndice],
						indiceCount);

			if (vertices->mDynamic)
				vertices->mVerticesOffset += indiceCount;

			return NO_ERROR;
		}

		void Graphics2D::clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			setTexture(mClearTexture);
			mClearSprite.color.r = r;
			mClearSprite.color.g = g;
			mClearSprite.color.b = b;
			mClearSprite.color.a = a;

			static const glm::mat4 ortho = glm::ortho(
				0.0f, (float) GraphicsBase::DISPLAY_WIDTH,
				(float) GraphicsBase::DISPLAY_HEIGHT, 0.0f,
				-1.0f, 1.0f);

			setProjectionMatrix(ortho);
			mClearBuffer->start();
			mClearBuffer->put(mClearSprite);
			render(mClearBuffer);
		}

	} // namespace Graphics

} // namespace NotEngine
