#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "barlight.h"

static bool running = true;

static void signalHandler(int signo)
{
	if (signo == SIGINT || signo == SIGTERM) {
		running = false;
	}
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	// Initialize abort handlers
	signal(SIGINT,  signalHandler);
	signal(SIGTERM, signalHandler);


	// Creating BarLightStrip object
	BarLightStrip barLightStrip;

	if (!barLightStrip.init()) {
		return 1;
	}

	barLightStrip.setColor(255, 128, 64);
	// Main loop
	int i=0;
	while (running) {
		// barLightStrip.setRainbowColor(i++);
		barLightStrip.render();
		usleep(20*1000);
	}
	return 0;
}
