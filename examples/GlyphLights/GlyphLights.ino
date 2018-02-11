// GlyphLights
// Render bit pictures in 8x32 Neopixel matrix and
// render shifting text.  (with bit font definitions)

#include <Adafruit_NeoPixel.h>
#include "PaletteFont.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

//#include <SPI.h>
//#include <SD.h>


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS         768
#define PIXELSPERCOLUMN     8
#define COLUMNSPERGLYPH     8
#define DISPLAYCOLUMNS   (NUMPIXELS/8)
#define NUMCOLORS           5


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Palette palette(String('mono'));
PaletteFont font(String('Samurai3'));

uint32_t colors[NUMCOLORS];

// Transform the data to serpentine to make the array look like the display
#define DATR(a, b, c, d, e, f, g, h)  h,g,f,e,d,c,b,a
#define DATF(a, b, c, d, e, f, g, h)  a,b,c,d,e,f,g,h

//
// Define the picture (along with a column for inbound and for outbound) data
// that could be used for shifting data into the image_data.   Not sure how I
// would be able to use that yet, but I think it might be handy for something.


/*
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
                            DATF(1, 1, 1, 1, 1, 1, 1, 1) };  // Outbound buffer of 8  */

                            
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
void shiftPicture(Adafruit_NeoPixel &pixels, const GlyphColumn &shift_in_column, uint32_t color) {
  for(int r=0; r<DISPLAYCOLUMNS; r++) {
    shiftColumn(pixels, r);
  }
  //
  //  Shift in some data into the first column (vacated by the shift)
  //  It is represented as a byte instead of an array...
  //
  byte sel;
  for(int i=0; i<PIXELSPERCOLUMN; i++) {
    if (shift_in_column.row(7-i) > 0) {
      pixels.setPixelColor(NUMPIXELS-i-1, color);
    } else {
      pixels.setPixelColor(NUMPIXELS-i-1, pixels.Color(0,0,0));
    }
  }

  pixels.show();
  delay(33);
}


//
// Reading image from pixel memory, shift it off to the right
//
void shiftPicture(Adafruit_NeoPixel &pixels, byte *shift_in_column, Palette &pal) {
  for(int r=0; r<DISPLAYCOLUMNS; r++) {
    shiftColumn(pixels, r);
  }
  //
  //  Shift in some data into the first column (vacated by the shift)
  //  It is represented as a byte instead of an array...
  //
  for(int i=0; i<PIXELSPERCOLUMN; i++) {
      Serial.print(shift_in_column[i]);
      Serial.print(" ");
      pixels.setPixelColor(NUMPIXELS-i-1, pal.color(shift_in_column[i]));
  }
  Serial.println("end of column");

  pixels.show();

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
//  from above.)   This was used initially for 8x8_basic.  Here for
//  posterity if it is needed for another bit-data kind of font.
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
  //loadPicture(pixels, picture);
  //delay(3000);

  // This is the CS pin on the 5100 board.
  //SD.begin(4);

  // Ths is required for MEGA compatibility with the SD reader?
  //pinMode(SS, OUTPUT);

  
}

//
//
void shiftInMessage(Adafruit_NeoPixel &pixels, char *message) {
    for (char m=' '; m!='~'; m++) {
      FontGlyph *glyph = font.glyph(m);

      // shift in the glyph one column at a time
      for (int col=0; col<COLUMNSPERGLYPH; col++) {
        char buf[32];
        sprintf(buf, "%c[%d]: 0x%x", m, col, glyph->column(col).data());
        Serial.println(buf);
        shiftPicture(pixels, glyph->column(col), pixels.Color(5, 0, 5));
      }
      // Put in a spacer between each letter
      shiftPicture(pixels, GlyphColumn(0), pixels.Color(0, 0, 0));    
      delay(33);
    }
}

void loop() {

  /*File myFile;

    // shift it down forever (pad with blank column before and after)
    //for (byte *each_column = letA.first_column();
    //           each_column != NULL;
    //           each_column = letA.next_column()) {
    //    shiftPicture(pixels, 1, *each_column, pixels.Color(7, 0, 7));
    //}
  // re-open the file for reading:
  myFile = SD.open("test.txt");

  if (myFile) {
    Serial.println("test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      char buffer[32];
      int bytes_read = myFile.read(buffer, sizeof(buffer)-1);
      buffer[bytes_read] = NULL;        // make sure it is NULL terminated
      shiftInMessage(pixels, buffer);
    }

    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  } */

  char *msg = "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 0 1 2 3 4 5 6 7 8 9";
  shiftInMessage(pixels, msg);

  delay(2000);

}