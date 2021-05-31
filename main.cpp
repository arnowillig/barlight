#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "rpi_ws281x/rpihw.h"
#include "barlight.h"
#include "restapi.h"
#include "hsv.h"

static bool running = true;
static unsigned int userCounter = 0;
static unsigned int mainCounter = 0;
static unsigned int beatCounter = 0;

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

	int overallBrightness = 255;
	int bri = 255;
	lightStrip.addSegment(new LightStripSegment( 74, 34, 1, bri)); // Right side
	lightStrip.addSegment(new LightStripSegment( 33, 28, 1, bri)); // Box short side
	lightStrip.addSegment(new LightStripSegment( 27,  9, 1, bri)); // Box long side
//	lightStrip.addSegment(new LightStripSegment(  0, 11, 0, bri)); // Inside box (12 leds)
	lightStrip.addSegment(new LightStripSegment( 12, 91, 0, bri)); // Back side
	lightStrip.addSegment(new LightStripSegment( 91,149, 0, 255)); // Bar lower
	lightStrip.addSegment(new LightStripSegment(208,150, 0, 255)); // Bar upper

	if (!lightStrip.init(overallBrightness)) {
		return 1;
	}
	fprintf(stderr, "Initialized %d (%d/%d) leds...\n", lightStrip.ledCount(), lightStrip.getMaxLedForChannel(0), lightStrip.getMaxLedForChannel(1));

	RESTServer api(&lightStrip);
	api.start(8080);

	lightStrip.clear();
	lightStrip.setMode("color");
	lightStrip.setColor(255, 146, 40);
	// Main loop
	int sleep = 10;
	int cnt = 0;
	int lc = lightStrip.ledCount();
	while (running) {
		if (lightStrip.mode()=="off") {
			sleep = 500;
			lightStrip.setColor(0, 0, 0);
		} else if (lightStrip.mode()=="rainbow") {
			sleep = 20;
			lightStrip.setRainbowColor(cnt);
		} else if (lightStrip.mode()=="color") {
			sleep = 100;
			lightStrip.setLastColor();
			
		} else if (lightStrip.mode()=="starlight") {
			sleep = 10;
			uint8_t r,g,b;
			float decay = 0.90;
			for (int i=0; i<lc; i++) {				
				lightStrip.getColor(i, &r, &g, &b);
				lightStrip.setColor(i, r*decay,g*decay,b*decay);
			}
			int i1 = rand() % lc;
			lightStrip.setColor(i1, 255, 255, 255);
			if (beatCounter != lightStrip.beatCounter()) {
				beatCounter = lightStrip.beatCounter();
				for (int i=0;i<50;i++) {
					int i1 = rand() % lc;
					lightStrip.setColor(i1, 255, 255, 255);
				}
			}
		
		} else if (lightStrip.mode()=="bluebeat") {
			sleep = 20;
			uint8_t r,g,b;
			float decay = 0.96;
			for (int i=0; i<lc; i++) {
				lightStrip.getColor(i, &r, &g, &b);
				lightStrip.setColor(i, r*decay,g*decay,b*decay);
			}
			if (beatCounter != lightStrip.beatCounter()) {
				beatCounter = lightStrip.beatCounter();
				lightStrip.setColor(0,0,255);
			}
		
		} else if (lightStrip.mode()=="snakes") {
			sleep = 10;
			uint8_t r,g,b;
			float decay = 0.75;
			for (int i=0; i<lc; i++) {				
				lightStrip.getColor(i, &r, &g, &b);
				lightStrip.setColor(i, r*decay,g*decay,b*decay);
			}
			int i1 = abs ((((uint8_t)(lc*0.0)+cnt) % (lc*2)) - lc);
			int i2 = abs ((((uint8_t)(lc*0.2)+cnt) % (lc*2)) - lc);
			int i3 = abs ((((uint8_t)(lc*0.4)+cnt) % (lc*2)) - lc);
			int i4 = abs ((((uint8_t)(lc*0.6)+cnt) % (lc*2)) - lc);
			int i5 = abs ((((uint8_t)(lc*0.8)+cnt) % (lc*2)) - lc);
			lightStrip.getColor(i1, &r, &g, &b);
			lightStrip.setColor(i1, 255, (128*g)/255, (128*b)/255);
			lightStrip.getColor(i2, &r, &g, &b);
			lightStrip.setColor(i2, (128*r)/255, 255, (128*b)/255);
			lightStrip.getColor(i3, &r, &g, &b);
			lightStrip.setColor(i3, (128*r)/255, (128*g)/255, 255);
			lightStrip.getColor(i4, &r, &g, &b);
			lightStrip.setColor(i4, 255, 255, (128*b)/255);
			lightStrip.getColor(i5, &r, &g, &b);
			lightStrip.setColor(i5, 255, (128*b)/255, 255);
		} else if (lightStrip.mode()=="disco") {
			sleep = 20;
			if (beatCounter != lightStrip.beatCounter()) {
				beatCounter = lightStrip.beatCounter();
				uint8_t r,g,b;
				for (int i=0; i<lightStrip.segmentCount(); i++) {
					uint8_t h = (rand() % 18) * 15;
					Color col = Color::fromHSV(h, 255, 255);
					// RgbColor rgb = hsvToRgb(h, 255, 255);
					r = col.r;
					g = col.g;
					b = col.b;
					LightStripSegment* segment = lightStrip.segment(i);
					segment->setColor(r,g,b);
				}
			}
		} else if (lightStrip.mode()=="snake") {
			sleep = 15;
			lightStrip.setLastColor();
			uint8_t r,g,b;
			for (int x=0;x<20;x++) {
				int ii = (cnt+x) % lightStrip.ledCount();
				uint8_t pos = (255*ii / (20-1) + cnt);
				lightStrip.calcRainbowColor(pos, r, g, b);
        		        lightStrip.setColor(ii, r, g, b);
				// lightStrip.setColor((cnt+x) % lightStrip.ledCount(), 255, 0, 0);
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
