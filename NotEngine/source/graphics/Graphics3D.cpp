#include "../../include/notengine/graphics/Graphics3D.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

extern const SceGxmProgram graphics3d_vert_gxp;
extern const SceGxmProgram graphics3d_frag_gxp;
static const SceGxmProgram* const s3dVertexProgramGxp	= &graphics3d_vert_gxp;
static const SceGxmProgram* const s3dFragmentProgramGxp	= &graphics3d_frag_gxp;

namespace NotEngine {

	namespace Graphics {

		Graphics3D::Graphics3D() : System::Singleton<Graphics3D>() {
		}

		Graphics3D::~Graphics3D() {
		}

		int Graphics3D::initialize() {
			GraphicsBase* base = GraphicsBase::instance();

			// Check if the 3d shaders are valids
			int err = sceGxmProgramCheck(s3dVertexProgramGxp);
			if (err != 0) {
				return VERTEX_SCEGXM_PROGRAM_CHECK;
			}
			err = sceGxmProgramCheck(s3dFragmentProgramGxp);
			if (err != 0) {
				return FRAGMENT_SCEGXM_PROGRAM_CHECK;
			}

			// register programs with the patcher
			m3dVertexProgramId = 0;
			err = sceGxmShaderPatcherRegisterProgram(base->mShaderPatcher, s3dVertexProgramGxp, &m3dVertexProgramId);
			if (err != 0) {
				return VERTEX_SCEGXM_REGISTER_PROGRAM;
			}

			m3dFragmentProgramId = 0;
			err = sceGxmShaderPatcherRegisterProgram(base->mShaderPatcher, s3dFragmentProgramGxp, &m3dFragmentProgramId);
			if (err != 0) {
				return FRAGMENT_SCEGXM_REGISTER_PROGRAM;
			}

			// get attributes by name to create vertex format bindings
			mShaderPositionAttr = sceGxmProgramFindParameterByName(s3dVertexProgramGxp, "aPosition");
			mShaderTextureAttr = sceGxmProgramFindParameterByName(s3dVertexProgramGxp, "aTexcoord");
			mShaderColorAttr = sceGxmProgramFindParameterByName(s3dVertexProgramGxp, "aColor");
			// gte uniforms
			mShaderMatrixProjUnif = sceGxmProgramFindParameterByName(s3dVertexProgramGxp, "pm");
			mShaderTextureEnableUnif = sceGxmProgramFindParameterByName(s3dFragmentProgramGxp, "textureEnable");

			// create the 2d vertex format
			SceGxmVertexAttribute g3dVertexAttributes[3];
			// Position fmt
			g3dVertexAttributes[0] = (SceGxmVertexAttribute) {
				.streamIndex = 0,
				.offset = 0,
				.format = SCE_GXM_ATTRIBUTE_FORMAT_F32,
				.componentCount = 3,
				.regIndex = (unsigned short) sceGxmProgramParameterGetResourceIndex(mShaderPositionAttr)
			};
			// Texture fmt
			g3dVertexAttributes[1] = (SceGxmVertexAttribute) {
				.streamIndex = 0,
				.offset = (unsigned short) sizeof(float)*3,
				.format = SCE_GXM_ATTRIBUTE_FORMAT_F32,
				.componentCount = 2,
				.regIndex = (unsigned short) sceGxmProgramParameterGetResourceIndex(mShaderTextureAttr)
			};
			// Color fmt
			g3dVertexAttributes[2] = (SceGxmVertexAttribute) {
				.streamIndex = 0,
				.offset = (unsigned short)  (g3dVertexAttributes[1].offset + sizeof(float)*2),
				.format = SCE_GXM_ATTRIBUTE_FORMAT_U8N,
				.componentCount = 4,
				.regIndex = (unsigned short) sceGxmProgramParameterGetResourceIndex(mShaderColorAttr)
			};
			// Indices fmt
			SceGxmVertexStream g3dVertexStreams[1] = {
				(SceGxmVertexStream) {
					.stride = sizeof(D3Buffer::D3Vertice),
					.indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT
				}
			};

			// create the vertex shaders
			err = sceGxmShaderPatcherCreateVertexProgram(
				base->mShaderPatcher,
				m3dVertexProgramId,
				g3dVertexAttributes,
				3,
				g3dVertexStreams,
				1,
				&m3dVertexProgram);
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
				m3dFragmentProgramId,
				SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
				(SceGxmMultisampleMode) GraphicsBase::MSAA_MODE,
				&blendInfo,
				s3dVertexProgramGxp,
				&m3dFragmentProgram);
			if (err != 0) {
				return FRAGMENT_SCEGXM_CREATE_PROGRAM;
			}

			mIndiceBuffer = new IndiceBuffer();
			err = mIndiceBuffer->initialize(Graphics3D::MAX_VERTICES_PER_BATCH);
			if (err != IndiceBuffer::NO_ERROR) {
				return INDICES_GPU_ALLOC;
			}

			// Fill the indices buffer as it will never change (used only for immediate rendering)
			mIndiceBuffer->start();
			for (unsigned short i=0; i<Graphics3D::MAX_VERTICES_PER_BATCH; i++) {
				mIndiceBuffer->put(i);
			}

			return NO_ERROR;
		}

		void Graphics3D::finalize() {
			GraphicsBase* base = GraphicsBase::instance();

			if (m3dFragmentProgramId != 0) {
				sceGxmShaderPatcherReleaseFragmentProgram(base->mShaderPatcher, m3dFragmentProgram);
				sceGxmShaderPatcherUnregisterProgram(base->mShaderPatcher, m3dFragmentProgramId);
			}

			if (m3dVertexProgramId != 0) {
				sceGxmShaderPatcherReleaseVertexProgram(base->mShaderPatcher, m3dVertexProgram);
				sceGxmShaderPatcherUnregisterProgram(base->mShaderPatcher, m3dVertexProgramId);
			}

			if (mIndiceBuffer != 0) {
				mIndiceBuffer->finalize();
				delete mIndiceBuffer;
			}
		}

		void Graphics3D::setTexture(const Texture2D* texture) const {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetFragmentTexture(base->mContext, 0, &texture->mTexture);
		}

		void Graphics3D::setProjectionMatrix(const glm::mat4& projection) const {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetUniformDataF(base->mVertexUniformDefaultBuffer, mShaderMatrixProjUnif, 0, 16, glm::value_ptr(projection));
		}

		void Graphics3D::use() const {
			GraphicsBase* base = GraphicsBase::instance();
			sceGxmSetVertexProgram(base->mContext, m3dVertexProgram);
			sceGxmSetFragmentProgram(base->mContext, m3dFragmentProgram);

			sceGxmSetFrontDepthWriteEnable(base->mContext, SCE_GXM_DEPTH_WRITE_ENABLED);
			sceGxmSetFrontDepthFunc(base->mContext, SCE_GXM_DEPTH_FUNC_LESS);

			sceGxmReserveVertexDefaultUniformBuffer(base->mContext, &base->mVertexUniformDefaultBuffer);
			sceGxmReserveFragmentDefaultUniformBuffer(base->mContext, &base->mFragmentUniformDefaultBuffer);
		}

		int Graphics3D::render(SceGxmPrimitiveType type, D3Buffer* vertices, bool texture) const {
			return render(type, mIndiceBuffer, vertices, texture);
		}

		int Graphics3D::render(SceGxmPrimitiveType type, IndiceBuffer* indices, D3Buffer* vertices, bool texture) const {
			unsigned int countInBuffer = vertices->mVerticesCount;
			switch(type) {
				// chek for errors
				case SCE_GXM_PRIMITIVE_TRIANGLES:
					if(countInBuffer % 3 != 0 || countInBuffer < 3) {
						return WRONG_VERTICES_COUNT;
					}
					break;
				case SCE_GXM_PRIMITIVE_TRIANGLE_EDGES: // not sure about this one
				case SCE_GXM_PRIMITIVE_TRIANGLE_FAN:
				case SCE_GXM_PRIMITIVE_TRIANGLE_STRIP:
					if(countInBuffer < 3) {
						return WRONG_VERTICES_COUNT;
					}
					break;
				case SCE_GXM_PRIMITIVE_LINES:
					if(countInBuffer % 2 != 0 || countInBuffer < 2) {
						return WRONG_VERTICES_COUNT;
					}
					break;
				case SCE_GXM_PRIMITIVE_POINTS:
					if(countInBuffer < 1) {
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
			sceGxmSetUniformDataF(base->mFragmentUniformDefaultBuffer, mShaderTextureEnableUnif, 0, 1, &textureEnable);

			unsigned int vertCount = countInBuffer - vertices->mVerticesOffset;
			sceGxmDraw(base->mContext,
						type,
						SCE_GXM_INDEX_FORMAT_U16,
						&indices->mIndices[vertices->mVerticesOffset],
						vertCount);

			if (vertices->mDynamic)
				vertices->mVerticesOffset += vertCount;

			return NO_ERROR;
		}

	} // namespace Graphics

} // namespace NotEngine
