#include "../../include/notengine/graphics/FrameCatalog.hpp"
#include "../../include/notengine/system/Utils.hpp"

#include <stdio.h>
#include <jsonxx.h>

using namespace NotEngine::System;

namespace NotEngine {

	namespace Graphics {

		FrameCatalog::FrameCatalog() : w(0), h(0) {
			printf("FrameCatalog()\n");
		}

		FrameCatalog::~FrameCatalog() {
			printf("~FrameCatalog()\n");
		}

		bool FrameCatalog::initialize(const std::string filename) {
			std::string jsonString = Utils::stringFromFile(filename);

			if (jsonString.length() == 0) {
				printf("FrameCatalog received empty string\n");
				return false;
			}

			jsonxx::Object json;
			if (!json.parse(jsonString + "\n")) {
				printf("jsonxx failed to parse: %s\n", filename.c_str());
				return false;
			}

			if (!json.has<jsonxx::Object>("frames") || !json.has<jsonxx::Object>("meta")
			|| !json.get<jsonxx::Object>("meta").has<jsonxx::Object>("size")
			|| !json.get<jsonxx::Object>("meta").get<jsonxx::Object>("size").has<jsonxx::Number>("w")
			|| !json.get<jsonxx::Object>("meta").get<jsonxx::Object>("size").has<jsonxx::Number>("h")) {
				printf("bad json, no meta or frames: %s\n", jsonString.c_str());
				return false;
			}
			w = json.get<jsonxx::Object>("meta").get<jsonxx::Object>("size").get<jsonxx::Number>("w");
			h = json.get<jsonxx::Object>("meta").get<jsonxx::Object>("size").get<jsonxx::Number>("h");


			std::map<std::string, jsonxx::Value*> framesMap = json.get<jsonxx::Object>("frames").kv_map();
			for(jsonxx::Object::container::const_iterator it = framesMap.begin(); it != framesMap.end(); ++it) {
				Frame frame;

				if (!it->second->get<jsonxx::Object>().has<jsonxx::Object>("frame") ||
					!it->second->get<jsonxx::Object>().has<jsonxx::Object>("sourceSize")) {
					printf("bad json, no frame or sourceSize: %s\n", filename.c_str());
					return false;
				}

				jsonxx::Object frameObject = it->second->get<jsonxx::Object>().get<jsonxx::Object>("frame");
				if (!frameObject.has<jsonxx::Number>("x") || !frameObject.has<jsonxx::Number>("y")
					|| !frameObject.has<jsonxx::Number>("w") || !frameObject.has<jsonxx::Number>("h")) {
					printf("bad json, bad frame: %s\n", filename.c_str());
					return false;
				}

				frame.s = frameObject.get<jsonxx::Number>("x") / w;
				frame.t = frameObject.get<jsonxx::Number>("y") / h;
				frame.u = (frameObject.get<jsonxx::Number>("x") + frameObject.get<jsonxx::Number>("w")) / w;
				frame.v = (frameObject.get<jsonxx::Number>("y") + frameObject.get<jsonxx::Number>("h")) / h;
				frame.w = frameObject.get<jsonxx::Number>("w");
				frame.h = frameObject.get<jsonxx::Number>("h");

				catalog.insert(
					std::pair<std::string, Frame>(it->first, frame)
				);
			}

			return true;
		}

		FrameCatalog::Frame FrameCatalog::getFrame(const std::string name) {
			if (catalog.find(name) == catalog.end()) {
				printf("Frame not found %s, throwing default frame.", name.c_str());
				return Frame();
			}

			return catalog[name];
		}

		void FrameCatalog::finalize() {
			catalog.clear();
			w = 0;
			h = 0;
		}

		int FrameCatalog::getWidth() {
			return w;
		}

		int FrameCatalog::getHeight() {
			return h;
		}

	} // namespace Graphics

} // namespace NotEngine
