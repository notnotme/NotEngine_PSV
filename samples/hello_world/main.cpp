#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/moduleinfo.h>
#include <psp2/kernel/processmgr.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <notengine/notengine.hpp>
#include <psp2shell.h>

PSP2_MODULE_INFO(0, 0, "hello_world")

using namespace NotEngine::Graphics;

/* main */
int main() {
	psp2shell_init(3333, 0);

	GraphicsBase* graphicsBase = GraphicsBase::instance();
	Graphics2D* graphics2D = Graphics2D::instance();
	bool ready = true;
	int ret;

	// Initialize base graphics system
	ret = graphicsBase->initialize(false);
	if (ret != GraphicsBase::NO_ERROR) {
		psp2shell_print("graphicsBase->initialize() fail: %d\n", ret);
		ready = false;
	}

	// Initialize the 2D sprite renderer
	if (ready) {
		ret = graphics2D->initialize();
		if (ret != Graphics2D::NO_ERROR) {
			psp2shell_print("graphics2D->initialize() fail: %d\n", ret);
			ready = false;
		}
	}

	// Create a buffer to prepare our sprite to render (64 sprite slot)
	SpriteBuffer* spriteBuffer = new SpriteBuffer();
	if (ready) {
		ret = spriteBuffer->initialize(64, true);
		if (ret != SpriteBuffer::NO_ERROR) {
			psp2shell_print("spriteBuffer->initialize() fail: %d\n", ret);
			ready = false;
		}
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
			ret = spriteBuffer->put(16,16,0, spriteLetter, "Hello World!");
			if (ret != SpriteBuffer::NO_ERROR) {
				psp2shell_print("Error: %d" + ret);
			}

			// set default texture then render
			graphics2D->setProjectionMatrix(ortho);
			graphics2D->setTexture(GraphicsBase::getDebugFontTexture());
			graphics2D->render(spriteBuffer);

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

	psp2shell_exit();
	sceKernelExitProcess(0);
	return 0;
}
