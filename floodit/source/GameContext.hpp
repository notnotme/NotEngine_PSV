#pragma once
#include <glm/glm.hpp>
#include <notengine/notengine.hpp>

using namespace NotEngine::System;
using namespace NotEngine::Graphics;

class GameContext : public Singleton<GameContext> {
	friend class Singleton<GameContext>;

	private:
		GameContext(const GameContext& copy);
		void operator=(GameContext const&);
		GameContext ();

		SpriteBuffer* mSpriteBuffer;
		Texture2D* mFloorTexture;
		Texture2D* mSpritesTexture;
		Texture2D* mFontTexture;
		FrameCatalog* mSpritesCatalog;
		glm::mat4 mOrtho;

		SpriteLetter mSpriteLetter;
		Sprite mFloorSprite;

	public:
		virtual ~GameContext ();

		int initialize();
		void finalize();

		Texture2D* getFloorTexture();
		Texture2D* getSpritesTexture();
		Texture2D* getFontTexture();
		FrameCatalog* getSpritesCatalog();
		glm::mat4* getOrthogonalMatrix();

		SpriteBuffer* getSpriteBuffer();
		Sprite* getFloorSprite();
		SpriteLetter* getSpriteLetter();

		enum ERROR {
			NO_ERROR = 0,
			FLOOR_TEXTURE = 1,
			SPRITES_TEXTURE = 2,
			FONT_TEXTURE = 3,
			SPRITE_CATALOG = 4,
			SPRITE_BUFFER = 5
		};

};
