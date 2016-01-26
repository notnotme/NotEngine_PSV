#ifndef FRAMECATALOG_HPP
#define	FRAMECATALOG_HPP

#pragma once
#include <string>
#include <map>

#include "Sprite.hpp"

using namespace NotEngine;

namespace NotEngine {

	namespace Graphics {

		// Base class to manage Frames
		class FrameCatalog {
			public:
				struct Frame {
					Sprite::SpriteFrame spriteFrame;
					unsigned short w;
					unsigned short h;

					Frame() : spriteFrame((Sprite::SpriteFrame) {0, 0, 0, 0}), w(32), h(32) {}
					Frame(Sprite::SpriteFrame frame, unsigned short w, unsigned short h) : spriteFrame(frame), w(w), h(h) {}
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
