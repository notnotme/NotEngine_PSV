#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <psp2/moduleinfo.h>
#include <notengine/notengine.hpp>

#include "GameContext.hpp"
#include "Title.hpp"
#include "Game.hpp"

PSP2_MODULE_INFO(0, 0, "floodit")

using namespace NotEngine::Graphics;
using namespace NotEngine::Game;

extern "C" {
	int scePowerSetArmClockFrequency(int freq);
}

/* main */
int main(int argc, char **argv) {
	bool ready = true;
	srand(time(0));
	scePowerSetArmClockFrequency(222); // scePowerSetArmClockFrequency(80); It seem to didn't work ? (= gpu bound?)

	GraphicsBase* graphicsBase = GraphicsBase::instance();
	Graphics2D* graphics2D = Graphics2D::instance();
	GameContext* gameContext = GameContext::instance();
	Director* director = Director::instance();
	Title* intro = new Title();
	Game* game = new Game();

	// Initialize base graphics system
#ifdef __DEBUG__
	if (ready && !graphicsBase->initialize(false)) {
#else
	if (ready && !graphicsBase->initialize(true)) {
#endif
		printf("graphicsBase->initialize() fail\n");
		ready = false;
	}

	// Initialize the 2D sprite renderer
	if (ready && !graphics2D->initialize()) {
		printf("graphics2D->initialize() fail\n");
		ready = false;
	}

	if(ready && !gameContext->initialize()) {
		printf("gameContext->initialize() fail\n");
		ready = false;
	}

	std::map<std::string, GameState*> states;
	states[intro->getName()] = intro;
	states[game->getName()] = game;
	if (ready && !director->initialize(states, intro->getName())) {
		printf("director->initialize() fail\n");
		ready = false;
	}

	if (ready) {
		while(director->isRunning()) {
			director->update();
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
	return 0;
}
