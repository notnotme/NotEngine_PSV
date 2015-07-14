#ifndef GRAPHICS2D_HPP
#define	GRAPHICS2D_HPP

#pragma once
#include <psp2/types.h>
#include <psp2/gxm.h>
#include <glm/glm.hpp>

#include "Sprite.hpp"
#include "Texture2D.hpp"
#include "../system/Singleton.hpp"

using namespace NotEngine;

namespace NotEngine {

	namespace Graphics {

		/**
		 * Base class that handle 2d graphics stuff.
		 */
		class Graphics2D : public System::Singleton<Graphics2D> {
			private:
				/// Disallow copy
				Graphics2D(const Graphics2D& copy);
				void operator=(Graphics2D const&);

				SceGxmShaderPatcherId g2dVertexProgramId;
				SceGxmShaderPatcherId g2dFragmentProgramId;

				const SceGxmProgramParameter* shaderPositionAttr;
				const SceGxmProgramParameter* shaderTextureAttr;
				const SceGxmProgramParameter* shaderColorAttr;
				const SceGxmProgramParameter* shaderAngleAttr;
				const SceGxmProgramParameter* shaderTranslationAttr;
				const SceGxmProgramParameter* shaderMatrixProjUnif;

				SceGxmVertexProgram* g2dVertexProgram;
				SceGxmFragmentProgram* g2dFragmentProgram;

				/// Internal sprite vertice layout
				struct SpriteVertice {
					float x; // vertices coords
					float y;
					float s; // tex coords
					float t;
					unsigned char r; // color
					unsigned char g;
					unsigned char b;
					unsigned char a;
					float angle; // angle
					float tx; // translation
					float ty;
				} __attribute__((packed));

				/// Pointer to use for sprite batching
				SpriteVertice* batchVertices;
				SceUID batchVerticesUID;

				unsigned short* batchIndices;
				SceUID batchIndicesUID;

				unsigned int batchCapacity;
				unsigned int batchOffset;
				unsigned int batchCount;

				Sprite clearSprite;
				Texture2D* clearTexture;

			protected:
				/// Friends class itself
				friend class System::Singleton<Graphics2D>;
				/// Disallow public instanciating
				Graphics2D ();

			public:
				virtual ~Graphics2D ();

				/* INIT **************************************************/
				/// Initialize 2d subsystem
				bool initialize(unsigned int batchSize);
				/// free the 2d subsystem
				void finalize();
				/// Call it before start drawing with g2d
				void use(const glm::mat4* projection);
				/// Call it after drawing
				void unuse();

				/* RENDER ************************************************/
				/// Clear the screen. be warned: this function actually unbind the current texture! Should be avoided if possible!!!
				void clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
				/// Set the current texture to use for render operation
				void setTexture(unsigned int unit, const Graphics::Texture2D* texture);
				/// Call it first in each render call
				void resetBatch();
				/// Begin a new batch of sprites
				void startBatch();
				/// Add a sprite in the batch
				void addToBatch(const Graphics::Sprite* sprite);
				/// Render all sprite added since startBatch sprite sprites
				void renderBatch();

		};

	} // namespace Graphics

} // namespace NotEngine

#endif	/* GRAPHICS2D_HPP */
