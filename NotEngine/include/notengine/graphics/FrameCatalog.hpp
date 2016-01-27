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
				struct FrameSize {
					unsigned int w,h;
				};

				struct FrameCoords {
					float s,t,u,v;
				};

				struct Frame {
					FrameSize size;
					FrameCoords coords;
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
