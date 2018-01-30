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
#define NUMCOLORS        5
#define NUMINBOUND       8
#define NUMOUTBOUND      8

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t colors[NUMCOLORS];

// Transform the data to serpentine to make the array look like the display
#define DATR(a, b, c, d, e, f, g, h)  h,g,f,e,d,c,b,a
#define DATF(a, b, c, d, e, f, g, h)  a,b,c,d,e,f,g,h

//
// Define the picture (along with a column for inbound and for outbound) data
// that could be used for shifting data into the image_data.   Not sure how I
// would be able to use that yet, but I think it might be handy for something.
//
byte image_data[NUMINBOUND+NUMPIXELS+NUMOUTBOUND] = 
                          { DATF(1, 1, 1, 1, 1, 1, 1, 1),   // InBound buffer of 8
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(0, 0, 1, 1, 1, 0, 1, 1),
                            DATF(0, 0, 0, 0, 0, 0, 0, 1),
                            DATR(0, 0, 0, 0, 0, 0, 0, 0),
                            DATF(0, 0, 0, 0, 0, 0, 0, 0),
                            DATR(0, 0, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 0, 0, 0, 0, 0, 0, 1),
                            DATF(0, 0, 0, 0, 0, 0, 0, 0),
                            DATR(0, 0, 2, 2, 2, 2, 0, 0),
                            DATF(0, 0, 2, 2, 2, 2, 0, 0),
                            DATR(0, 0, 0, 0, 0, 0, 0, 0),
                            DATF(1, 0, 0, 0, 0, 0, 0, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),
                            DATR(2, 2, 2, 2, 2, 2, 2, 2), };  // Outbound buffer of 8

// 8 element array of stuff to be shifted in
byte *inbound = image_data;
// N element array containing the picture data
byte *picture = image_data+NUMINBOUND;
// 8 element array of stuff that was just shifted out
byte *outbound = image_data+NUMINBOUND+NUMPIXELS;
                            
void loadSerpentinePicture(Adafruit_NeoPixel &pixels, byte *picture) {
  //
  // If data is stored in Serpentine form, then we can just
  // load it in.  If the DATF and DATR macros are used, then
  // the array is in serpentine order.
  //
  for (int i=0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, colors[picture[i]]);
  }

  pixels.show();
}

void loadPicture(Adafruit_NeoPixel &pixels, byte *picture) {
  loadSerpentinePicture(pixels, picture);
}


int delayval = 1;

#define NUMROWS 8

//
//  This loads an array by providing the serpentine transform
//  as it is loaded.   Every other line has to be reversed as
//  it is loaded in the pixel matrix.
//
void loadArrayPicture(Adafruit_NeoPixel &pixels, byte *picture) {

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

//
// Reading image from pixel memory, shift it off to the right
//
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

  colors[0] = pixels.Color(0,  0,  3);
  colors[1] = pixels.Color(2,  1,  0);    //  red
  colors[2] = pixels.Color(0,  2,  0);    //  green (zig zag?)
  colors[3] = pixels.Color(0,  0,  3);    //  blue
  colors[4] = pixels.Color(5,  5,  0);

  Serial.begin(9600);

  // For a set of NeoPixels the first NeoPixel is 0, second is 1
  loadPicture(pixels, picture);
}

void loop() {
    // shift it down forever
    // shiftPicture(pixels, 1);
}