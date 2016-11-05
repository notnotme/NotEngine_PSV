#ifndef GRAPHICS2D_HPP
#define	GRAPHICS2D_HPP

#pragma once
#include <psp2/types.h>
#include <psp2/gxm.h>
#include <glm/glm.hpp>

#include "../system/Singleton.hpp"
#include "Texture2D.hpp"
#include "Sprite.hpp"
#include "SpriteBuffer.hpp"

namespace NotEngine {

	namespace Graphics {

		/**
		 * Base class that handle 2d graphics stuff.
		 */
		class Graphics2D : public System::Singleton<Graphics2D> {
			friend class System::Singleton<Graphics2D>;

			private:
				/// Disallow copy
				Graphics2D(const Graphics2D& copy);
				void operator=(Graphics2D const&);

				SceGxmShaderPatcherId m2dVertexProgramId;
				SceGxmShaderPatcherId m2dFragmentProgramId;

				const SceGxmProgramParameter* mShaderPositionAttr;
				const SceGxmProgramParameter* mShaderTextureAttr;
				const SceGxmProgramParameter* mShaderColorAttr;
				const SceGxmProgramParameter* mShaderAngleAttr;
				const SceGxmProgramParameter* mShaderTranslationAttr;
				const SceGxmProgramParameter* mShaderMatrixProjUnif;

				SceGxmVertexProgram* m2dVertexProgram;
				SceGxmFragmentProgram* m2dFragmentProgram;

				unsigned short* mBatchIndices;
				SceUID mBatchIndicesUID;

				/// Objects to use for clear operation
				SpriteBuffer* mClearBuffer;
				Sprite mClearSprite;
				Texture2D* mClearTexture;

				/// Disallow public instanciating
				Graphics2D ();

			public:
				// (screen is 960x544px, for 16x16px tiles, it is 540 tiles per layer). Should be okay
				static const unsigned int MAX_SPRITES_PER_BATCH = 2048;

				virtual ~Graphics2D ();

				/* INIT **************************************************/
				/// Initialize 2d subsystem
				int initialize();
				/// free the 2d subsystem
				void finalize();
				/// Call it before start drawing with g2d
				void use() const;

				/* RENDER ************************************************/
				/// Clear the screen. be warned: this function actually unbind the current texture and buffer! Should be avoided if possible or use your own "clear sprite" !!!
				void clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
				/// Set the current texture to use for render operation
				void setTexture(const Graphics::Texture2D* texture) const;
				/// Set the projection matrix
				void setProjectionMatrix(const glm::mat4& projection) const;
				/// Render all sprite added in the buffer since the last call to draw the batch
				void render(Graphics::SpriteBuffer* spriteBuffer) const;

				enum ERROR {
					NO_ERROR = 0,
					VERTEX_SCEGXM_PROGRAM_CHECK = 1,
					FRAGMENT_SCEGXM_PROGRAM_CHECK = 2,
					VERTEX_SCEGXM_REGISTER_PROGRAM = 3,
					FRAGMENT_SCEGXM_REGISTER_PROGRAM = 4,
					VERTEX_SCEGXM_CREATE_PROGRAM = 5,
					FRAGMENT_SCEGXM_CREATE_PROGRAM = 6,
					INDICES_GPU_ALLOC = 7,
					CLEAR_TEXTURE_INITIALIZE = 8,
					CLEAR_BUFFER_INITIALIZE = 9
				};

		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* GRAPHICS2D_HPP */
