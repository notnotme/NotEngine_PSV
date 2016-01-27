#include "../../include/notengine/graphics/Sprite.hpp"

namespace NotEngine {

	namespace Graphics {

		Sprite::Sprite() {
			frame = (FrameCatalog::Frame) {
				.size = (FrameCatalog::FrameSize) {32, 32},
				.coords = (FrameCatalog::FrameCoords) {0, 0, 1, 1}
			};

			position = (SpritePosition) {
				.x = 0,
				.y = 0
			};

			scale = (SpriteScale) {
				.w = 1,
				.h = 1
			};

			color = (SpriteColor) {
				.r = 255,
				.g = 255,
				.b = 255,
				.a = 255
			};

			rotation = 0;
		}

		Sprite::~Sprite() {
		}

	} // namespace Graphics

} // namespace NotEngine
