#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <psp2/power.h>
#include <psp2/kernel/processmgr.h>
#include <notengine/notengine.hpp>
#include <psp2shell.h>

#include "GameContext.hpp"
#include "Title.hpp"
#include "Game.hpp"

using namespace NotEngine::Graphics;
using namespace NotEngine::Game;

bool psp2shell = false;

/* main */
int main(int argc, char **argv) {
#ifdef DEBUG
	if (psp2shell_init(3333, 0) == 0) {
		psp2shell = true;
	}
#endif

	bool ready = true;
	int ret;
	srand(time(0));
	scePowerSetArmClockFrequency(222);

	GraphicsBase* graphicsBase = GraphicsBase::instance();
	Graphics2D* graphics2D = Graphics2D::instance();
	GameContext* gameContext = GameContext::instance();
	Director* director = Director::instance();
	Title* intro = new Title();
	Game* game = new Game();

	// Initialize base graphics system
	ret = graphicsBase->initialize(false);
	if (ret != GraphicsBase::NO_ERROR) {
#ifdef DEBUG
		if (psp2shell) psp2shell_print("graphicsBase->initialize() fail: %d", ret);
#endif
		ready = false;
	}
	// Initialize the 2D sprite renderer
	if (ready) {
		ret = graphics2D->initialize();
		if (ret != Graphics2D::NO_ERROR) {
#ifdef DEBUG
			if (psp2shell) psp2shell_print("graphics2D->initialize() fail: %d\n", ret);
#endif
			ready = false;
		}
	}

	if(ready) {
		ret = gameContext->initialize();
		if (ret != GameContext::NO_ERROR) {
#ifdef DEBUG
			if (psp2shell) psp2shell_print("gameContext->initialize() fail: %s\n", ret);
#endif
			ready = false;
		}
	}

	std::map<std::string, GameState*> states;
	states[intro->getName()] = intro;
	states[game->getName()] = game;
	if (ready) {
		ret = director->initialize(states, intro->getName());
		if (ret != Director::NO_ERROR) {
#ifdef DEBUG
			if (psp2shell) psp2shell_print("director->initialize() fail: %d\n", ret);
#endif
			ready = false;
		}
	}

	if (ready) {
		while(director->isRunning()) {
			ret = director->update();
			if (ret != 0) {
#ifdef DEBUG
				if (psp2shell) psp2shell_print("director->update() fail: %d\n", ret);
#endif
				ready = false;
			}
		}
	}

	graphicsBase->waitTerminate();

	director->finalize();
	gameContext->finalize();
	graphics2D->finalize();
	graphicsBase->finalize();
	delete intro;
	delete game;

	Director::deleteInstance();
	GameContext::deleteInstance();
	Graphics2D::deleteInstance();
	GraphicsBase::deleteInstance();

#ifdef DEBUG
	psp2shell_exit();
	sceKernelExitProcess(0);
#endif

	return 0;
}
