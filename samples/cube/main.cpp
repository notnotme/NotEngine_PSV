#include <psp2/types.h>
#include <psp2/ctrl.h>
#include <psp2/moduleinfo.h>
#include <psp2/kernel/processmgr.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <notengine/notengine.hpp>
#include <psp2shell.h>

PSP2_MODULE_INFO(0, 0, "cube")

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
	ret = graphicsBase->initialize(true);
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

	// Create a buffer for the vertices
	D3Buffer* d3Buffer = new D3Buffer();
	if (ready) {
		ret = d3Buffer->initialize(Graphics3D::MAX_VERTICES_PER_BATCH, false);
		if (ret != D3Buffer::NO_ERROR) {
			psp2shell_print("d3Buffer->initialize() fail: %d\n", ret);
			ready = false;
		}
	}
	d3Buffer->start();
	d3Buffer->put(-1.0f, -1.0f,  1.0f,  255,   0,   0, 255);
	d3Buffer->put( 1.0f, -1.0f,  1.0f,    0, 255,   0, 255);
	d3Buffer->put( 1.0f,  1.0f,  1.0f,    0,   0, 255, 255);
	d3Buffer->put(-1.0f,  1.0f,  1.0f,  255, 255, 255, 255);
	d3Buffer->put(-1.0f, -1.0f, -1.0f,  255,   0,   0, 255);
	d3Buffer->put( 1.0f, -1.0f, -1.0f,    0, 255,   0, 255);
	d3Buffer->put( 1.0f,  1.0f, -1.0f,    0,   0, 255, 255);
	d3Buffer->put(-1.0f,  1.0f, -1.0f,  255, 255, 255, 255);

	// Create an indice buffer for vertices
	IndiceBuffer* indiceBuffer = new IndiceBuffer();
	if (ready) {
		ret = indiceBuffer->initialize(36);
		if (ret != IndiceBuffer::NO_ERROR) {
			psp2shell_print("indiceBuffer->initialize() fail: %d\n", ret);
			ready = false;
		}
	}
	indiceBuffer->start();
	indiceBuffer->put(0); indiceBuffer->put(1);
	indiceBuffer->put(2); indiceBuffer->put(2);
	indiceBuffer->put(3); indiceBuffer->put(0);

	indiceBuffer->put(1); indiceBuffer->put(5);
	indiceBuffer->put(6); indiceBuffer->put(6);
	indiceBuffer->put(2); indiceBuffer->put(1);

	indiceBuffer->put(7); indiceBuffer->put(6);
	indiceBuffer->put(5); indiceBuffer->put(5);
	indiceBuffer->put(4); indiceBuffer->put(7);

	indiceBuffer->put(4); indiceBuffer->put(0);
	indiceBuffer->put(3); indiceBuffer->put(3);
	indiceBuffer->put(7); indiceBuffer->put(4);

	indiceBuffer->put(4); indiceBuffer->put(5);
	indiceBuffer->put(1); indiceBuffer->put(1);
	indiceBuffer->put(0); indiceBuffer->put(4);

	indiceBuffer->put(3); indiceBuffer->put(2);
	indiceBuffer->put(6); indiceBuffer->put(6);
	indiceBuffer->put(7); indiceBuffer->put(3);

	glm::mat4 mv;
	glm::mat4 persp;
	glm::mat4 mvp;

	SceCtrlData pad;
	float angle = 0.0f;
	while(ready) {
		angle += 0.02f;
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
		 * glm::rotate(mv, angle, glm::vec3(1.0f, 1.0f, 1.0f));

		mvp = persp * mv;

		// Start rendering
		graphicsBase->startDrawing();
			graphics2D->use();
			graphics2D->clear(0x00, 0x00, 0x00, 0xff);

			graphics3D->use();
			graphics3D->render(mvp, SCE_GXM_PRIMITIVE_TRIANGLES, indiceBuffer, d3Buffer, false, 0, 36);

		graphicsBase->stopDrawing();
		graphicsBase->swapBuffers();
	}

	// You must wait for the renderer to terminate before anything
	graphicsBase->waitTerminate();

	d3Buffer->finalize();
	delete d3Buffer;

	indiceBuffer->finalize();
	delete indiceBuffer;

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
