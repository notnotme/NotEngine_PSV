#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <notengine/notengine.hpp>
#include <psp2shell.h>

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

	// Create a buffer to prepare our shapes to render (64 vertices slot)
	D2Buffer* d2Buffer = new D2Buffer();
	if (ready) {
		ret = d2Buffer->initialize(64, true);
		if (ret != D2Buffer::NO_ERROR) {
			psp2shell_print("d2Buffer->initialize() fail: %d\n", ret);
			ready = false;
		}
	}

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

			d2Buffer->start();
			d2Buffer->put(100, 100,  255,   0,   0, 255);
			d2Buffer->put(100, 150,    0,   0, 255, 255);
			d2Buffer->put(150, 150,  255, 255, 255, 255);
			d2Buffer->put(150, 100,    0, 255,   0, 255);
			graphics2D->render(ortho, SCE_GXM_PRIMITIVE_TRIANGLE_FAN, d2Buffer, false);

			d2Buffer->put(300, 300,  255, 0, 0, 255);
			d2Buffer->put(400, 400,  255, 0, 0, 255);
			d2Buffer->put(400, 300,  255, 0, 0, 255);
			d2Buffer->put(300, 400,  255, 0, 0, 255);
			graphics2D->render(ortho, SCE_GXM_PRIMITIVE_LINES, d2Buffer, false);

			d2Buffer->put(600, 300,  255, 0, 255, 255);
			d2Buffer->put(600, 450,  255, 0, 255, 255);
			d2Buffer->put(800, 450,  255, 0, 255, 255);
			d2Buffer->put(800, 300,  255, 0, 255, 255);
			graphics2D->render(ortho, SCE_GXM_PRIMITIVE_TRIANGLE_FAN, d2Buffer, false);

			d2Buffer->put(600, 100,  0, 0, 255, 255);
			d2Buffer->put(600, 250,  0, 0, 255, 255);
			d2Buffer->put(600, 250,  0, 0, 255, 255);
			d2Buffer->put(800, 250,  0, 0, 255, 255);
			d2Buffer->put(800, 250,  0, 0, 255, 255);
			d2Buffer->put(800, 100,  0, 0, 255, 255);
			d2Buffer->put(800, 100,  0, 0, 255, 255);
			d2Buffer->put(600, 100,  0, 0, 255, 255);
			graphics2D->render(ortho, SCE_GXM_PRIMITIVE_LINES, d2Buffer, false);

		// stop and swap display
		graphicsBase->stopDrawing();
		graphicsBase->swapBuffers();
	}

	// You must wait for the renderer to terminate before anything
	graphicsBase->waitTerminate();

	// cleanup then quit
	d2Buffer->finalize();
	delete d2Buffer;

	graphics2D->finalize();
	graphicsBase->finalize();

	Graphics2D::deleteInstance();
	GraphicsBase::deleteInstance();

	psp2shell_exit();
	sceKernelExitProcess(0);
	return 0;
}
