#pragma once
#include <notengine/notengine.hpp>

using namespace NotEngine::Graphics;

class GameSprite : public Sprite {

	public:
		unsigned int intColor;

		GameSprite();
		virtual ~GameSprite ();

		void setIntColor(unsigned int intColor);
		unsigned int getIntColor();

};
