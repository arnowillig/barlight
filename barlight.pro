QT =

CONFIG += c++17 console
CONFIG -= app_bundle

MAKEFILE = Makefile_qmake
OTHER_FILES += Makefile html/*

DEFINES += _GUI_

SOURCES += \
	barlight.cpp \
	main.cpp \
	restapi.cpp \
	hsv.cpp \
	rpi_ws281x/dma.c rpi_ws281x/mailbox.c rpi_ws281x/pcm.c rpi_ws281x/pwm.c rpi_ws281x/rpihw.c rpi_ws281x/ws2811.c

HEADERS += \
	barlight.h \
	restapi.h \
	hsv.h \
	rpi_ws281x/clk.h rpi_ws281x/dma.h rpi_ws281x/gpio.h rpi_ws281x/mailbox.h rpi_ws281x/pcm.h rpi_ws281x/pwm.h rpi_ws281x/rpihw.h rpi_ws281x/ws2811.h


LIBS += -lpistache -lssl -lcrypto
