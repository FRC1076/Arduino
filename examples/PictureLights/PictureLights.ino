// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      256
#define NUMCOLORS        3

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int colors[NUMCOLORS];

byte picture[NUMPIXELS] = { 2, 2, 2, 2, 2, 2, 2, 2,
                            1, 1, 1, 1, 1, 1, 1, 2,     // 0  - 7
                            1, 1, 1, 1, 1, 1, 2, 1,
                            1, 1, 1, 1, 1, 2, 1, 1,
                            1, 1, 1, 1, 2, 1, 1, 1,
                            1, 1, 1, 2, 1, 1, 1, 1,
                            1, 1, 2, 1, 1, 1, 1, 1,
                            1, 2, 1, 1, 1, 1, 1, 1,
                            2, 1, 1, 1, 1, 1, 1, 1,
                            1, 2, 1, 1, 1, 1, 1, 1,
                            1, 1, 2, 1, 1, 1, 1, 1,
                            1, 1, 1, 2, 1, 1, 1, 1,
                            1, 1, 1, 1, 2, 1, 1, 1,
                            1, 1, 1, 1, 1, 2, 1, 1,
                            1, 1, 1, 1, 1, 1, 2, 1,
                            1, 1, 1, 1, 1, 1, 1, 2,
                            1, 1, 1, 1, 1, 1, 2, 1,
                            1, 1, 1, 1, 1, 2, 1, 1,
                            1, 1, 1, 1, 2, 1, 1, 1,
                            1, 1, 1, 2, 1, 1, 1, 1,
                            1, 1, 2, 1, 1, 1, 1, 1,
                            1, 2, 1, 1, 1, 1, 1, 1,
                            2, 1, 1, 1, 1, 1, 1, 1,
                            1, 2, 1, 1, 1, 1, 1, 1,
                            1, 1, 2, 1, 1, 1, 1, 1,
                            1, 1, 1, 2, 1, 1, 1, 1,
                            1, 1, 1, 1, 2, 1, 1, 1,
                            1, 1, 1, 1, 1, 2, 1, 1,
                            1, 1, 1, 1, 1, 1, 2, 1,
                            1, 1, 1, 1, 1, 1, 1, 2,
                            2, 2, 2, 2, 2, 2, 2, 2,
                            2, 2, 2, 2, 2, 2, 2, 2 };
                            

void loadPicture(Adafruit_NeoPixel &pixels, byte *picture) {

  int num_groups = pixels.numPixels() / 16;

  for (int group=0; group < num_groups; group++) {
    // reverse the first 8 in the group so it loads like the data looks
    int btm = group*16;
    int top = btm + 7;
    for (int i=0; i<8; i++) {
      pixels.setPixelColor(top-i, colors[picture[btm+i]]);
    }

    // work from a new base 8 pixels higher and just do 1 to 1 transfer
    btm += 8;
    for (int i=0; i<8; i++) {
      pixels.setPixelColor(btm+i, colors[picture[btm+i]]);
    }
  }

  pixels.show();
}

int delayval = 1;

#define NUMROWS 8

void shiftPicture(Adafruit_NeoPixel &pixels, int how_far) {
  for(int r=31; r>=0; r--) {
    shiftRow(pixels, r);
  }
}

void shiftRow(Adafruit_NeoPixel &pixels, int group) {
  
  // toss in order column data to ooo column (and vice versa)
  int dest_btm = group*8;
  int dest_top = dest_btm+7;
  int src_btm = dest_btm-8;
  int src_top = src_btm+8;
  for (int i=0; i<NUMROWS; i++) {
     int p = pixels.getPixelColor(src_btm+i);
     pixels.setPixelColor(dest_top-i, p);
  }
  pixels.show();
}

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.

  colors[0] = pixels.Color(0,  0,  0);
  colors[1] = pixels.Color(5,  0,  5);
  colors[2] = pixels.Color(0,  5,  0);

  Serial.begin(9600);

  // For a set of NeoPixels the first NeoPixel is 0, second is 1
  loadPicture(pixels, picture);
}

void loop() {
    // shift it down forever
    shiftPicture(pixels, 1);
    delay(1);
}