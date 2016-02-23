#include <stdio.h>
#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/moduleinfo.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <notengine/notengine.hpp>

PSP2_MODULE_INFO(0, 0, "triangle")

using namespace NotEngine::Graphics;

/* main */
int main() {
	GraphicsBase* graphicsBase = GraphicsBase::instance();
	Graphics2D* graphics2D = Graphics2D::instance();
	Graphics3D* graphics3D = Graphics3D::instance();
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

	// Initialize the 3D renderer
	if (ready && !graphics3D->initialize()) {
		printf("graphics3D->initialize() fail\n");
		ready = false;
	}

	// Create a buffer for the immediate vertices
	D3Buffer* d3Buffer = new D3Buffer();
	if (ready && !d3Buffer->initialize(Graphics3D::MAX_VERTICES_PER_BATCH, true)) {
		printf("d3Buffer failed to allocate\n");
		ready = false;
	}

	glm::mat4 persp = glm::perspective(
		75.0f,
		(float) GraphicsBase::DISPLAY_WIDTH/GraphicsBase::DISPLAY_HEIGHT,
		1.0f, 1024.0f);

	SceCtrlData pad;
	while(ready) {
		// Read controls and touchscreen
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons & SCE_CTRL_START) {
			ready = false;
		}

		// Start rendering
		graphicsBase->startDrawing();
			graphics2D->use();
			graphics2D->clear(0x00, 0x00, 0x00, 0xff);

			graphics3D->use();
			graphics3D->setTexture(GraphicsBase::getDebugFontTexture());
			graphics3D->setProjectionMatrix(persp);
			d3Buffer->start();
			d3Buffer->put( 1, 1,-10,  0.0f,0.0f,  255,0,255,255);
			d3Buffer->put(-1,-1,-10,  1.0f,1.0f,  255,255,0,255);
			d3Buffer->put( 1,-1,-10,  1.0f,0.0f,  0,255,255,255);
			graphics3D->render(SCE_GXM_PRIMITIVE_TRIANGLES, d3Buffer, true);

		graphicsBase->stopDrawing();
		graphicsBase->swapBuffers();
	}

	// You must wait for the renderer to terminate before anything
	graphicsBase->waitTerminate();

	d3Buffer->finalize();
	delete d3Buffer;

	graphics3D->finalize();
	graphics2D->finalize();
	graphicsBase->finalize();

	Graphics3D::deleteInstance();
	Graphics2D::deleteInstance();
	GraphicsBase::deleteInstance();
	return 0;
}
