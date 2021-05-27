#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "barlight.h"

static bool running = true;
static unsigned int userCounter = 0;

static void signalHandler(int signo)
{
	if (signo == SIGINT || signo == SIGTERM) {
		running = false;
	}
	if (signo == SIGUSR1) {
		userCounter++;
	}
	if (signo == SIGUSR2) {
		userCounter--;
	}
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	// Initialize abort handlers
	signal(SIGINT,  signalHandler);
	signal(SIGTERM, signalHandler);


	// Creating LightStrip object
	LightStrip lightStrip;

	lightStrip.addSegment(new LightStripSegment( 75, 50, 1)); // Seite
	lightStrip.addSegment(new LightStripSegment( 50, 40, 1)); // Box schmale Seite
	lightStrip.addSegment(new LightStripSegment( 40, 10, 1)); // Box breite Seite
	lightStrip.addSegment(new LightStripSegment( 12,100, 0)); // Hinten
	lightStrip.addSegment(new LightStripSegment(100,150, 0)); // Bar unten
	// lightStrip.addSegment(new LightStripSegment(150,200, 0)); // Bar oben

	if (!lightStrip.init()) {
		return 1;
	}

	lightStrip.setColor(255, 128, 64);
	// Main loop
	int cnt = 0;
	while (running) {
		int seg = userCounter % lightStrip.segmentCount();

		lightStrip.clear();
		lightStrip.segment(seg)->setColor(255, 128, 64);
		lightStrip.segment(seg)->setColor(0, 255,0,0); // First RED
		lightStrip.segment(seg)->setColor(lightStrip.segment(seg)->ledCount()-1, 0, 255,0); // Last GREEN

		// barLightStrip.setColor(bar)
		// barLightStrip.setRainbowColor(cnt);
		lightStrip.render();
		usleep(20*1000);

		cnt++;
	}
	return 0;
}
