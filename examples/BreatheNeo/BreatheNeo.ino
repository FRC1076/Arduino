#include <Adafruit_NeoPixel.h>

#define NUM_LEDS 64
#define PIN 6

#define STEPS_PER_SECOND  30
#define BREATHE_SECONDS    4
#define BREATHE_STEPS      (STEPS_PER_SECOND * BREATHE_SECONDS)
#define RADIANS_PER_STEP   ((2 * 3.1416) / BREATHE_STEPS)

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
byte brightness[BREATHE_STEPS];

// vary these to acheive different dynamic range
// Brightness will vary between MinBrightness
// and MaxBrightness
byte MaxBrightness = 200;
byte MinBrightness = 10;
byte BrightnessScale = (MaxBrightness - MinBrightness) / 2;
bool DumpBrightnessValues = false;

// This assumes we take no time to display lights
// We may need to adjust this down to get target steps per second
float EstimatedWorkTimePerStep = 100.0;
float StepDelay = (1000.0 - EstimatedWorkTimePerStep) / STEPS_PER_SECOND;

void setup() {
    if (DumpBrightnessValues) {
        Serial.begin(115200);
    }

    // Turn off all of the lights
  	strip.begin();
  	strip.clear();
    strip.show();

    // set up the sign wave data for time efficiency
    for (int step=0; step<BREATHE_STEPS; step++) {
        float intensity = MinBrightness + BrightnessScale * (1 + sin(RADIANS_PER_STEP * step));
        brightness[step] = byte(trunc(intensity));

        if (DumpBrightnessValues) {
            Serial.print("Brightness[");
            Serial.print(step);
            Serial.print("] =");
            Serial.println(brightness[step]);
        }
    }
}

void loop() {
	//Written by: Jason Yandell
    //Rewritten by: Matthew Jones
    //
    //  Do all steps of the breathe cycle
	for (int step = 0; step < BREATHE_STEPS; step++) {
        // use precomputed brightness values
        strip.setBrightness(brightness[step]);
        
        // Set every LED to some purplish color
	    for (int ledNumber=0; ledNumber<NUM_LEDS; ledNumber++) {
		    strip.setPixelColor(ledNumber, Adafruit_NeoPixel::Color(50,0,50));
	    }
        strip.show();

		//Wait a bit before continuing to breathe
		delay(StepDelay);
    }
}