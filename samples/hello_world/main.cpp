#include <stdio.h>
#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/moduleinfo.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <notengine/notengine.hpp>

PSP2_MODULE_INFO(0, 0, "hello_world")

using namespace NotEngine::Graphics;

/* main */
int main() {
	GraphicsBase* graphicsBase = GraphicsBase::instance();
	Graphics2D* graphics2D = Graphics2D::instance();
	bool ready = true;

	// Initialize base graphics system
	if (ready && !graphicsBase->initialize(false)) {
		printf("graphicsBase->initialize() fail\n");
		ready = false;
	}

	// Initialize the 2D sprite renderer
	if (ready && !graphics2D->initialize()) {
		printf("graphics2D->initialize() fail\n");
		ready = false;
	}

	// Create a buffer to prepare our sprite to render (64 sprite slot)
	SpriteBuffer* spriteBuffer = new SpriteBuffer();
	if (!spriteBuffer->initialize(64)) {
		printf("spriteBuffer failed to allocate\n");
		ready = false;
	}

	// Prepare our sprite model
	SpriteLetter spriteLetter;
	spriteLetter.frame.size = (FrameCatalog::FrameSize) {32, 32}; // letters size
	spriteLetter.color = (Sprite::SpriteColor) {0,0,0,255}; // letters color

	// prepare the orthogonal matrix
	glm::mat4 ortho = glm::ortho(
		0.0f, (float) GraphicsBase::DISPLAY_WIDTH,
		(float) GraphicsBase::DISPLAY_HEIGHT, 0.0f,
		-1.0f, 1.0f);

	SceCtrlData pad;
	while(ready) {
		// Read controls and touchscreen
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons & SCE_CTRL_START) {
			ready = false;
		}

		// Start rendering
		graphicsBase->startDrawing();
			// to call before any 2d rendering
			graphics2D->use();
			// you should avoid to call clear, but it is available
			graphics2D->clear(0xff, 0xff, 0xff, 0xff);

			// call start when you will render into the buffer the first time
			spriteBuffer->start();
			spriteBuffer->put(16,16,0, &spriteLetter, "Hello World!");

			// set default texture then render
			graphics2D->setTexture(0, GraphicsBase::getDebugFontTexture());
			graphics2D->render(&ortho, spriteBuffer);

			/*
			graphics2D->setTexture(0, myTexture);
			spriteBuffer->put(&one);
			spriteBuffer->put(&two);
			spriteBuffer->put(&three);
			graphics2D->render(&ortho, spriteBuffer);
			*/
		// stop and swap display
		graphicsBase->stopDrawing();
		graphicsBase->swapBuffers();
	}

	// You must wait for the renderer to terminate before anything
	graphicsBase->waitTerminate();

	// cleanup then quit
	spriteBuffer->finalize();
	delete spriteBuffer;

	graphics2D->finalize();
	graphicsBase->finalize();

	Graphics2D::deleteInstance();
	GraphicsBase::deleteInstance();
	return 0;
}
