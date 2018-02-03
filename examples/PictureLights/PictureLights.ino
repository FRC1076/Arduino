// PictureLights
// Render bit pictures in 8x32 Neopixel matrix and
// render shifting text.  (with bit font definitions)

#include <Adafruit_NeoPixel.h>
#include "FontLetter.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      256
#define PIXELSPERCOLUMN  8
#define NUMCOLUMNS       (256/8)
#define NUMCOLORS        5


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t colors[NUMCOLORS];

byte fontbits[] =                 { 0b11111000,   // F8
                                    0b11111110,
                                    0b00110011,
                                    0b00110011,
                                    0b11111110,
                                    0b11111000  };

FontLetter letA = FontLetter('A', fontbits, 6);


// Transform the data to serpentine to make the array look like the display
#define DATR(a, b, c, d, e, f, g, h)  h,g,f,e,d,c,b,a
#define DATF(a, b, c, d, e, f, g, h)  a,b,c,d,e,f,g,h

//
// Define the picture (along with a column for inbound and for outbound) data
// that could be used for shifting data into the image_data.   Not sure how I
// would be able to use that yet, but I think it might be handy for something.


byte picture[NUMPIXELS] = 
                          {    // InBound buffer of 8
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
                            DATR(0, 0, 1, 1, 1, 1, 0, 0),
                            DATF(0, 0, 1, 1, 1, 1, 0, 0),
                            DATR(0, 0, 0, 0, 0, 0, 0, 0),
                            DATF(1, 0, 0, 0, 0, 0, 0, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 2, 1),
                            DATR(2, 2, 2, 1, 1, 1, 2, 2),
                            DATF(2, 2, 2, 2, 1, 1, 2, 2),
                            DATR(1, 1, 1, 2, 2, 1, 2, 2),
                            DATF(1, 1, 1, 1, 2, 2, 2, 2),
                            DATR(1, 1, 1, 1, 1, 2, 2, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1),                            
                            DATR(1, 0, 0, 0, 0, 0, 0, 1),
                            DATF(0, 0, 0, 0, 0, 0, 0, 0),
                            DATR(0, 0, 1, 1, 0, 1, 0, 0),
                            DATF(0, 0, 1, 1, 0, 1, 0, 0),
                            DATR(0, 0, 0, 0, 0, 1, 0, 0),
                            DATF(1, 0, 0, 0, 1, 1, 0, 1),
                            DATR(1, 1, 1, 1, 1, 1, 1, 1),
                            DATF(1, 1, 1, 1, 1, 1, 1, 1) };  // Outbound buffer of 8

                            
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

//
//  This loads an array by providing the serpentine transform
//  as it is loaded.   Every other line has to be reversed as
//  it is loaded in the pixel matrix.
//
void loadArrayPicture(Adafruit_NeoPixel &pixels, byte *picture) {

  int num_groups = NUMPIXELS / 16;

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
void shiftPicture(Adafruit_NeoPixel &pixels, int how_far, byte shift_in_column, uint32_t color) {
  for(int r=0; r<32; r++) {
    shiftColumn(pixels, r);
  }
  //
  //  Shift in some data into the first column (vacated by the shift)
  //  It is represented as a byte instead of an array...
  //
  byte sel;
  for(int i=0,sel=0x1; i<PIXELSPERCOLUMN; i++,sel=sel<<1) {
    if (shift_in_column & sel) {
      pixels.setPixelColor(NUMPIXELS-i-1, color);
    } else {
      pixels.setPixelColor(NUMPIXELS-i-1, pixels.Color(0,0,0));
    }
  }

  pixels.show();
  delay(33);
}

void shiftColumn(Adafruit_NeoPixel &pixels, int column) {
  
  // toss in order column data to ooo column (and vice versa)
  int src_btm = column*8;           //-- first in column
  int src_top = src_btm+7;          //-- last in column
  int dest_btm = src_btm-8;         //-- first in dest column
  int dest_top = dest_btm+7;        //-- last in dest column

  //
  // transfer data starting at first src element (increasing index)
  // to the location starting at first dest (decreasing index)
  // This is general enough that it works for zig or zag columns
  //
  for (int i=0; i<PIXELSPERCOLUMN; i++) {
     uint32_t p = pixels.getPixelColor(src_btm+i);
     pixels.setPixelColor(dest_top-i, p);
  }
}

//
//  Rotate the font 90 degrees to the left.   This makes
//  it suitable for shifting in from the right (instead of
//  from above.)
//
void rotate_font(byte *font, byte *rotated) {
  byte sel = 0x1;    // select the 1 bit from each and shift into result
  for (int r=7; r>=0; r--,sel<<=1) {
    byte result = 0x0;
    for (int i=7; i>=0; i--) {
      if (font[i] & sel) {
        result = (result << 1) | 0x1;   // shift up and set bottom bit
      } else {
        result = (result << 1);
      }
    }
    rotated[r] = result;
  }
}

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.

  colors[0] = pixels.Color(10,  0,  3);
  colors[1] = pixels.Color(0,  0,  0);    //  red
  colors[2] = pixels.Color(1,  1,  3);    //  green (zig zag?)
  colors[3] = pixels.Color(0,  0,  3);    //  blue
  colors[4] = pixels.Color(5,  5,  0);

  Serial.begin(9600);

  // For a set of NeoPixels the first NeoPixel is 0, second is 1
  loadPicture(pixels, picture);
  delay(3000);
}

//
//  Note:   Font definition is still a global imported with the FontLetter class.
//  Perhaps we can do a better job and encapsulate the font in a proper class.
//  Class could provide a way to set the color for all following uses of the font,
//  and could hide things like the -32 offset and the array representation...
//
void shiftInMessage(Adafruit_NeoPixel &pixels, char *message, uint32_t msg_size) {
    for (int letter = 0; letter < msg_size; letter++) {
      byte *glyph = font8x8_1076[message[letter]-32];

      // shift in the glyph one column at a time
      for (int col=0; col<8; col++) {
        shiftPicture(pixels, 1, glyph[col], pixels.Color(50, 20, 0));
      }
    }
}

void loop() {
    // shift it down forever (pad with blank column before and after)
    //for (byte *each_column = letA.first_column();
    //           each_column != NULL;
    //           each_column = letA.next_column()) {
    //    shiftPicture(pixels, 1, *each_column, pixels.Color(7, 0, 7));
    //}
    char msg[] = "I'm sorry, Dave. I'm afraid I can't do that.";
    shiftInMessage(pixels, msg, sizeof(msg));

    delay(2000);

    char msg2[] = "Wow! that was fast!";
    shiftInMessage(pixels, msg2, sizeof(msg2));

    delay(2000);

    char msg3[] = "Ouch!!!!";
    shiftInMessage(pixels, msg3, sizeof(msg3));


}