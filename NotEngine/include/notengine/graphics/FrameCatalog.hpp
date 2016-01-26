#ifndef FRAMECATALOG_HPP
#define	FRAMECATALOG_HPP

#pragma once
#include <string>
#include <map>

namespace NotEngine {

	namespace Graphics {

		// Base class to manage Frames
		class FrameCatalog {
			public:
				struct Frame {
					float s;
					float t;
					float u;
					float v;

					unsigned short w;
					unsigned short h;

					Frame() : s(0), t(0), u(1), v(1), w(32), h(32) {}
				};

			private:
				unsigned int mWidth;
				unsigned int mHeight;
				std::string mTextureName;
				std::map<std::string, Frame> mCatalog;

			public:
				FrameCatalog();
				virtual ~FrameCatalog();

				bool initialize(const std::string filename);
				void finalize();
				Frame getFrame(const std::string name);

				int getWidth();
				int getHeight();

				std::string getTextureName();
		};

	} // namespace Graphics

} // namespace NotEngine

#endif /* FRAMECATALOG_HPP */
