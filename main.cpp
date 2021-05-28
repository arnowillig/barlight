#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "rpi_ws281x/rpihw.h"
#include "barlight.h"
#include "restapi.h"

static bool running = true;
static unsigned int userCounter = 0;
static unsigned int mainCounter = 0;

static void signalHandler(int signo)
{
	switch (signo) {
	case SIGINT:
	case SIGTERM:
		running = false;
		break;
	case SIGUSR1:
		userCounter++;
		break;
	case SIGUSR2:
		userCounter--;
		break;
	case SIGHUP:
		mainCounter = 0;
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	// Initialize abort handlers
	signal(SIGINT,  signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGHUP,  signalHandler);
	signal(SIGUSR1, signalHandler);
	signal(SIGUSR2, signalHandler);
	
	// Initialize random generator
	srand(time(NULL));

	// Creating LightStrip object
	LightStrip lightStrip;

	int overallBrightness = 40;
	int bri = 255;
	lightStrip.addSegment(new LightStripSegment( 74, 34, 1, bri)); // Right side
	lightStrip.addSegment(new LightStripSegment( 33, 28, 1, bri)); // Box short side
	lightStrip.addSegment(new LightStripSegment( 27,  9, 1, bri)); // Box long side
//	lightStrip.addSegment(new LightStripSegment(  0, 11, 0, bri)); // Inside box (12 leds)
	lightStrip.addSegment(new LightStripSegment( 12, 91, 0, bri)); // Back side
	lightStrip.addSegment(new LightStripSegment( 91,149, 0, 255)); // Bar lower
	lightStrip.addSegment(new LightStripSegment(150,211, 0, 255)); // Bar upper

	if (!lightStrip.init(overallBrightness)) {
		return 1;
	}
	
/*
#ifndef _GUI_
	const rpi_hw_t* hw = rpi_hw_detect();
	if (hw) {
		fprintf(stderr, "Running on %s...\n", hw->desc);
	}
#endif
*/
	fprintf(stderr, "Initialized %d leds...\n", lightStrip.ledCount());

	RESTServer api(&lightStrip);
	api.start(8080);

	lightStrip.clear();
	lightStrip.setMode("snake");
	lightStrip.setColor(255, 170, 170);
	// Main loop
	int sleep = 10;
	int cnt = 0;
	while (running) {
		lightStrip.setColor(255, 170, 40);
		if (lightStrip.mode()=="off") {
			sleep = 500;
			lightStrip.setColor(0, 0, 0);
		} else if (lightStrip.mode()=="rainbow") {
			sleep = 5;
			lightStrip.setRainbowColor(cnt);
		} else if (lightStrip.mode()=="color") {
			sleep = 100;
			lightStrip.setLastColor();
		} else if (lightStrip.mode()=="snake") {
			sleep = 5;
			lightStrip.setLastColor();
			for (int x=0;x<10;x++) {
				lightStrip.setColor((cnt+x) % lightStrip.ledCount(), 255, 255, 255);
			}
		}
		lightStrip.render();
		usleep(sleep*1000);

		cnt++;
	}
	fprintf(stderr, "BarLight shutting down..\n");
	lightStrip.setColor(0, 0, 0);
	lightStrip.render();
	
	return 0;
}
