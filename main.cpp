#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "barlight.h"
#include "restapi.h"

static bool running = true;
static unsigned int userCounter = 4;

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

static void startServer(RESTServer* restServer)
{
	restServer->start(8080);
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	// Initialize abort handlers
	signal(SIGINT,  signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGUSR1, signalHandler);
	signal(SIGUSR2, signalHandler);
	
	// Initialize random generator
	srand(time(NULL));


	// Creating LightStrip object
	LightStrip lightStrip;

	int bri = 48; // 48
	lightStrip.addSegment(new LightStripSegment( 74, 34, 1, 255)); // Seite
	lightStrip.addSegment(new LightStripSegment( 33, 28, 1, 255)); // Box schmale Seite
	lightStrip.addSegment(new LightStripSegment( 27,  9, 1, 255)); // Box breite Seite
//	lightStrip.addSegment(new LightStripSegment(  0, 91, 0, 255)); // Hinten
	lightStrip.addSegment(new LightStripSegment( 12, 91, 0, 255)); // Hinten
	lightStrip.addSegment(new LightStripSegment( 91,149, 0, 255)); // Bar unten
	lightStrip.addSegment(new LightStripSegment(150,211, 0, 255)); // Bar oben

	if (!lightStrip.init(bri)) {
		return 1;
	}
	
	fprintf(stderr, "Initialized %d leds...\n", lightStrip.ledCount());

	RESTServer api(&lightStrip);
	std::thread restThread(&startServer, &api);

	lightStrip.clear();
	// 
	lightStrip.setColor(255, 255, 255);
	// Main loop
	int cnt = 0;
	while (running) {
		fprintf(stderr, "%d\n", cnt);
		lightStrip.setColor(255, 170, 40);
//
//
//		lightStrip.setRainbowColor(cnt);
//		int seg = (cnt/50) % lightStrip.segmentCount();
/*
		lightStrip.clear();
		lightStrip.segment(seg)->setColor(255, 128, 64);
		lightStrip.segment(seg)->setColor(0, 255,0,0); // First RED
		lightStrip.segment(seg)->setColor(lightStrip.segment(seg)->ledCount()-1, 0, 255,0); // Last GREEN
*/
//		for (int seg=0; seg < lightStrip.segmentCount(); seg++) {
//			lightStrip.segment(seg)->setColor(255,255,255);
////			lightStrip.segment(seg)->setColor(255,170,40)
//			lightStrip.segment(seg)->setRainbowColor(cnt);
//		}
//		lightStrip.setColor(255,170,40);
//		lightStrip.setColor(255,255,255);
//
//
		for (int x=0;x<10;x++) {
		
			lightStrip.setColor((cnt+x) % lightStrip.ledCount(), 255, 255, 255);
		}
		/*
		if (rand() % 10 == 1) {
			int seg = rand() % lightStrip.segmentCount();
			lightStrip.segment(seg)->setColor(255,0,0);
		}
		*/
		/*
		for (int seg=0; seg < lightStrip.segmentCount(); seg++) {
			lightStrip.segment(seg)->setRainbowColor(cnt);
		}
		*/
#endif
		lightStrip.render();
		usleep(10*1000);

		cnt++;
	}
	return 0;
}
