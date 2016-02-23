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
					/// Size in pixel of the frame
					FrameSize size;
					/// Texture coordinate
					FrameCoords coords;
				};

			private:
				/// Width of the texture catalog
				unsigned int mWidth;
				/// Height of the texture catalog
				unsigned int mHeight;
				/// filename of the texture
				std::string mTextureName;
				/// The hashmap that store thr frames
				std::map<std::string, Frame> mCatalog;

			public:
				FrameCatalog();
				virtual ~FrameCatalog();

				/// Load a catalog from a file
				bool initialize(const std::string jsonString);
				/// cleanup a catalog and free datas
				void finalize();
				/// Return a spriteframe of the name name
				Frame getFrame(const std::string name);

				/// Getters
				unsigned int getWidth() const;
				unsigned int getHeight() const;
				std::string getTextureName() const;
		};

	} // namespace Graphics

} // namespace NotEngine

#endif /* FRAMECATALOG_HPP */
