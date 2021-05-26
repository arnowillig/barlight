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

	barLightStrip.setColor(255, 255, 255);
	// Main loop
	while (running) {
		barLightStrip.render();
		usleep(40*1000);
	}
	return 0;
}
