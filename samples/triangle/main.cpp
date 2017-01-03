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
	Graphics3D* graphics3D = Graphics3D::instance();
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

	// Initialize the 3D renderer
	if (ready) {
		ret = graphics3D->initialize();
		if (ret != Graphics3D::NO_ERROR) {
			psp2shell_print("graphics3D->initialize() fail: %d\n", ret);
			ready = false;
		}
	}

	// Create a buffer for the immediate vertices
	D3Buffer* d3Buffer = new D3Buffer();
	if (ready) {
		ret = d3Buffer->initialize(Graphics3D::MAX_VERTICES_PER_BATCH, true);
		if (ret != D3Buffer::NO_ERROR) {
			psp2shell_print("d3Buffer->initialize() fail: %d\n", ret);
			ready = false;
		}
	}

	glm::mat4 mv;
	glm::mat4 persp;
	glm::mat4 mvp;

	SceCtrlData pad;
	float angle = 0.0f;
	while(ready) {
		angle += 0.01f;
		// Read controls and touchscreen
		sceCtrlPeekBufferPositive(0, &pad, 1);
		if (pad.buttons & SCE_CTRL_START) {
			ready = false;
		}

		persp = glm::perspective(
			75.0f,
			(float) GraphicsBase::DISPLAY_WIDTH/GraphicsBase::DISPLAY_HEIGHT,
			1.0f, 1024.0f);

		mv = glm::mat4();
		mv = glm::translate(mv, glm::vec3(0.0f, 0.0f, -10.0f))
		 * glm::rotate(mv, angle, glm::vec3(1.0f, 0.5f, 1.0f));

		mvp = persp * mv;

		// Start rendering
		graphicsBase->startDrawing();
			graphics2D->use();
			graphics2D->clear(0x00, 0x00, 0x00, 0xff);

			graphics3D->use();
			graphics3D->setTexture(GraphicsBase::getDebugFontTexture());
			d3Buffer->start();
			d3Buffer->put( 1.0f, 1.0f,0.0f,  0.0f,0.0f,  255,0,255,255);
			d3Buffer->put(-1.0f,-1.0f,0.0f,  1.0f,1.0f,  255,255,0,255);
			d3Buffer->put( 1.0f,-1.0f,0.0f,  1.0f,0.0f,  0,255,255,255);
			graphics3D->render(mvp, SCE_GXM_PRIMITIVE_TRIANGLES, d3Buffer, true);

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

	psp2shell_exit();
	sceKernelExitProcess(0);
	return 0;
}
