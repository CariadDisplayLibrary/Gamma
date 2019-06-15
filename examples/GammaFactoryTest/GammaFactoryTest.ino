#include <DTWI.h>
#include <I2S.h>
#include <TLV320DAC3101.h>
#include <DSDVOL.h>
#include <DebouncedInput.h>
#include <Gamma.h>
#include <WavData.h>
#include <AnalogTouchMZ.h>
#include <CP437.h>
#include <Framebuffer565.h>
#include <Widgets.h>

#pragma board gamma
#pragma programmer pic32prog
#pragma core chipkit
#pragma port /dev/ttyACM0

DTWI1 twi;

I2S i2s(44100);
TLV320DAC3101 dac(twi, 44100, PIN_DAC_RESET);

DSPI2 sdSPI;
DSDVOL vol(sdSPI, PIN_SD_CS);

GammaTFT tft;
AnalogTouchMZ ts(PIN_LEFT, PIN_RIGHT, PIN_UP, PIN_DOWN, 320, 480);

color_t buffer[320 * 480];
Framebuffer565 fb(320, 480, buffer);

twButton Yes(ts, tft, 160, 440, 160, 40, "Yes");
twButton No(ts, tft, 0, 440, 160, 40, "No");

WavData leftChannel(audio_channel_side_left_data);
WavData rightChannel(audio_channel_side_right_data);

void startTest(const char *msg) {
	fb.setTextColor(Color::White);
	fb.print("[....] ");
	fb.print(msg);
	fb.draw(tft, 0, 0);
}

void passTest() {
	fb.print("\r");
	fb.setTextColor(Color::White);
	fb.print("[");
	fb.setTextColor(Color::Green);
	fb.println("PASS");
	fb.setTextColor(Color::White);
	fb.draw(tft, 0, 0);
}

void failTest() {
	fb.print("\r");
	fb.setTextColor(Color::White);
	fb.print("[");
	fb.setTextColor(Color::Red);
	fb.println("FAIL");
	fb.setTextColor(Color::White);
	fb.draw(tft, 0, 0);
}

bool run;
bool yesnoResponse;

void respondYes(Event *e) {
	run = false;
	yesnoResponse = true;
}

void respondNo(Event *e) {
	run = false;
	yesnoResponse = false;
}


bool yesno(const char *message, bool draw) {
	tft.setFont(Fonts::CP437);
	tft.fillScreen(Color::Black);
	tft.setTextColor(Color::Yellow, Color::Black);
	tft.setCursor(0, 0);
	tft.print(message);
	Yes.onTap(respondYes);
	No.onTap(respondNo);
	run = true;
	Yes.redraw();
	No.redraw();
	while (run) {
		ts.sample();
		if (draw) {
			if (ts.isPressed()) {
				tft.fillCircle(ts.x(), ts.y(), 2, Color::Green);
			}
		}
		Yes.render();
		No.render();
	}
	return yesnoResponse;
}

void setup() {
	tft.initializeDevice();
	tft.fillScreen(Color::Black);
	tft.setBacklight(255);

	fb.initializeDevice();
	fb.fillScreen(Color::Black);

	fb.setCursor(0, 0);
	fb.setTextColor(Color::Yellow);
	fb.setFont(Fonts::CP437);
	fb.println("     Project Gamma Factory Test");
	fb.println("     ==========================");
	fb.println();
	fb.draw(tft, 0, 0);
	
	startTest("TFT Display");
	passTest();

	startTest("Touch screen");
	ts.initializeDevice();
	ts.scaleX(10);
	ts.offsetX(-45);
	
	ts.scaleY(7.5);
	ts.offsetY(-30);
	if (yesno("Does touchscreen work?", true)) {
		passTest();
	} else {
		failTest();
	}

	startTest("Initialize DAC");
	i2s.begin();
	dac.begin();
	dac.setGain(127);
	dac.setVolume(180);	
	dac.setLineOutMode();
	dac.setStereo();
	passTest();

	startTest("Mount SD Card");
	FRESULT fr = DFATFS::fsmount(vol, "0:", 1);
	if (fr == FR_OK) {
		passTest();
	} else {
		failTest();
	}

	startTest("Play Sound");
	int p = i2s.playLeft(&leftChannel, 1.0);
	while (i2s.isPlaying(p)) {
		yield();
	}
	p = i2s.playRight(&rightChannel, 1.0);
	while (i2s.isPlaying(p)) {
		yield();
	}
	if (yesno("Did you hear both channels?", false)) {
		passTest();
	} else {
		failTest();
	}

	startTest("User Button");
	pinMode(PIN_BTN1, INPUT_PULLUP);
	delay(10);
	if (digitalRead(PIN_BTN1) == LOW) {
		failTest();
	} else {
		while (digitalRead(PIN_BTN1) == HIGH);
		passTest();
	}
}

void loop() {
}
