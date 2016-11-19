#ifndef GRAPHICS3D_HPP
#define	GRAPHICS3D_HPP

#pragma once
#include <psp2/types.h>
#include <psp2/gxm.h>
#include <glm/glm.hpp>

#include "../system/Singleton.hpp"
#include "Texture2D.hpp"
#include "IndiceBuffer.hpp"
#include "D3Buffer.hpp"

namespace NotEngine {

	namespace Graphics {

		/**
		 * Base class that handle 3d graphics stuff.
		 */
		class Graphics3D : public System::Singleton<Graphics3D> {
			friend class System::Singleton<Graphics3D>;

			private:
				/// Disallow copy
				Graphics3D(const Graphics3D& copy);
				void operator=(Graphics3D const&);

				SceGxmShaderPatcherId m3dVertexProgramId;
				SceGxmShaderPatcherId m3dFragmentProgramId;

				const SceGxmProgramParameter* mShaderPositionAttr;
				const SceGxmProgramParameter* mShaderTextureAttr;
				const SceGxmProgramParameter* mShaderColorAttr;
				const SceGxmProgramParameter* mShaderMatrixProjUnif;
				const SceGxmProgramParameter* mShaderTextureEnableUnif;

				SceGxmVertexProgram* m3dVertexProgram;
				SceGxmFragmentProgram* m3dFragmentProgram;

				IndiceBuffer* mIndiceBuffer;

				/// Disallow public instanciating
				Graphics3D ();

			public:
				// allow 4096 vertice per draw call
				static const unsigned int MAX_VERTICES_PER_BATCH = 4096;

				virtual ~Graphics3D ();

				/* INIT **************************************************/
				/// Initialize 2d subsystem
				int initialize();
				/// free the 2d subsystem
				void finalize();
				/// Call it before start drawing with g2d
				void use();
				/* RENDER ************************************************/
				/// Set the current texture to use for render operation
				void setTexture(const Texture2D* texture) const;
				/// Render a D3Buffer using vertice type (optional texture)
				int render(const glm::mat4& projection, SceGxmPrimitiveType type, D3Buffer* vertices, bool texture);
				/// Render a D3Buffer using vertice type and using an index buffer (optional texture)
				int render(const glm::mat4& projection, SceGxmPrimitiveType type, IndiceBuffer* indices, D3Buffer* vertices, bool texture, int startIndice, int indiceCount);

				enum ERROR {
					NO_ERROR = 0,
					VERTEX_SCEGXM_PROGRAM_CHECK = 1,
					FRAGMENT_SCEGXM_PROGRAM_CHECK = 2,
					VERTEX_SCEGXM_REGISTER_PROGRAM = 3,
					FRAGMENT_SCEGXM_REGISTER_PROGRAM = 4,
					VERTEX_SCEGXM_CREATE_PROGRAM = 5,
					FRAGMENT_SCEGXM_CREATE_PROGRAM = 6,
					INDICES_GPU_ALLOC = 7,
					WRONG_VERTICES_COUNT = 8,
					UNSUPPORTED_FEATURE = 9
				};

		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* GRAPHICS3D_HPP */
