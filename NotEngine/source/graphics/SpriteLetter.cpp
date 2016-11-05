#include "../../include/notengine/graphics/SpriteLetter.hpp"
#include "../../include/notengine/graphics/GraphicsBase.hpp"

namespace NotEngine {

	namespace Graphics {

		SpriteLetter::SpriteLetter() : Sprite() {
			mFrames = GraphicsBase::getFontFrames();
		}

		SpriteLetter::~SpriteLetter() {
			mFrames = 0;
		}

		void SpriteLetter::setFrame(const char letter) {
			if (mFrames != 0) {
				frame.coords = mFrames[(int) letter].coords;
			}
		}

	} // namespace Graphics

} // namespace NotEngine
