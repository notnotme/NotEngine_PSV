#include "../../include/notengine/system/Utils.hpp"

#include <cstdio>
#include <fstream>
#include <cmath>

namespace NotEngine {

	namespace System {

		Utils::Utils() {
		}

		Utils::~Utils() {
		}

		std::string Utils::stringFromFile(const std::string filename) {
			std::ifstream myfile(filename.c_str());
			if (!myfile.is_open()) {
				printf("Failed to open file: %s\n", filename.c_str());
				return "";
			}

			std::string text;
			std::string line;
			while (getline (myfile, line)) {
				text += line;
			}
			myfile.close();

			return text;
		}

		bool Utils::isTouched(int touchX, int touchY, const Graphics::Sprite& sprite) {
			return (std::abs(touchX - sprite.position.x) < (sprite.frame.size.w*0.5f)
				&& std::abs(touchY - sprite.position.y) < (sprite.frame.size.h*0.5f));
		}

	} // namespace System

} // namespace NotEngine
