#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 64
#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  	strip.begin();
  	strip.setBrightness(16);  	// Lower brightness and save eyeballs!
  	strip.clear();
  	strip.show(); 				// Initialize all pixels to 'off'
}

void loop() {
	//Written by: Jason Yandell

	float MaximumBrightness = 64;
	float SpeedFactor = 3.1416 / 600; // I don't actually know what would look good
	float StepDelay = 20; // ms for a step delay on the lights

    // First set every LED to some purplish color
	for (int ledNumber=0; ledNumber<NUM_LEDS; ledNumber++) {
		strip.setPixelColor(ledNumber, 20, 0, 20);
	}

    // Vary the intensity over the fully range.
	// Make the lights breathe
	for (int i = 0; i < 300; i++) {
		// Intensity will go from 10 - MaximumBrightness in a "breathing" manner
		float intensity = MaximumBrightness * (1.0 + sin(SpeedFactor * i));
		strip.setBrightness(intensity);
		strip.show();
		//Wait a bit before continuing to breathe
		delay(StepDelay);
	}
}