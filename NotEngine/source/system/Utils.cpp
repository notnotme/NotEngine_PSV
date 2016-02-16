#include "../../include/notengine/system/Utils.hpp"

#include <cstdio>
#include <fstream>

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

	} // namespace System

} // namespace NotEngine
