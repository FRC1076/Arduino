// PictureLights
// Render bit pictures in 8x32 Neopixel matrix and
// render shifting text.  (with 4-bit (16 color) img definitions)

#include <Adafruit_NeoPixel.h>
#include "PaletteFont.h"

#ifdef __AVR__
  #include <avr/power.h>
#endif

//#include <SPI.h>
//#include <SD.h>


// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN0            6
#define PIN1            7
#define PIN2            8

//
// How many NeoPixels are attached to each Pin?
//
#define NUMPIXELS         256
#define PIXELSPERCOLUMN     8
#define COLUMNSPERGLYPH     8
#define DISPLAYCOLUMNS   (NUMPIXELS/PIXELSPERCOLUMN)

#define NEO_COLOR(r,g,b)    (uint32_t)(((uint32_t)r<<16)|((uint32_t)g<<8)|(uint32_t)b)
#define NUMDISPLAYS         3


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels[NUMDISPLAYS] = {
        Adafruit_NeoPixel(NUMPIXELS, PIN0, NEO_GRB + NEO_KHZ800),
        Adafruit_NeoPixel(NUMPIXELS, PIN1, NEO_GRB + NEO_KHZ800),
        Adafruit_NeoPixel(NUMPIXELS, PIN2, NEO_GRB + NEO_KHZ800),
};

/* This array is a simple representation of the color palette we use. */
/* uint32_t colors[] = {
        0x0,
        0xb8b8b3,
        0xa92a2a,
        0xece8b5,
        0xfffff9,
}; */

uint32_t colors[] = {
        0x000000,
        0x100027,
};

#define NUMCOLORS  (sizeof(colors)/sizeof(uint32_t))

#define PICTURECOLUMNS 22

/* GlyphColumn32 supports 16 color encoding so the
 * enter 8-bit column fits into 32bit quantity.
 * This means only 1024 bytes for a full panel of image
 * data and 64bytes for a full palette of 16 colors.
 */
GlyphColumn32 column_data[NUMDISPLAYS][PICTURECOLUMNS] = {
  {
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(1, 1, 0, 0, 0, 0, 0, 1),
    GC32(1, 0, 0, 0, 0, 0, 1, 1),
    GC32(1, 0, 0, 0, 0, 0, 1, 1),
    GC32(1, 1, 1, 1, 1, 1, 0, 0),
    GC32(0, 1, 0, 0, 0, 1, 1, 1),
    GC32(0, 1, 1, 0, 0, 0, 1, 1),
    GC32(0, 0, 0, 0, 0, 1, 1, 1),
    GC32(0, 0, 0, 0, 0, 0, 0, 1),
    GC32(0, 0, 0, 0, 0, 1, 1, 1),
    GC32(0, 0, 0, 0, 1, 1, 1, 1),
    GC32(0, 1, 0, 0, 1, 1, 1, 1),
    GC32(0, 1, 1, 0, 0, 1, 1, 1),
    GC32(0, 1, 1, 0, 0, 0, 0, 1),
    GC32(0, 0, 1, 1, 1, 1, 1, 0),
    GC32(0, 1, 1, 0, 0, 0, 0, 0),
    GC32(0, 1, 0, 0, 0, 0, 0, 0),
    GC32(0, 1, 1, 0, 0, 0, 0, 0),
  },
  {
    GC32(0, 0, 0, 0, 1, 1, 0, 0),
    GC32(0, 0, 0, 0, 0, 1, 1, 1),
    GC32(0, 0, 0, 1, 1, 0, 1, 1),
    GC32(0, 1, 0, 0, 1, 1, 0, 1),
    GC32(0, 1, 1, 1, 0, 1, 1, 1),
    GC32(0, 1, 1, 1, 1, 0, 1, 1),
    GC32(1, 0, 1, 1, 1, 1, 0, 1),
    GC32(1, 1, 0, 1, 1, 1, 1, 1),
    GC32(1, 1, 0, 0, 1, 1, 1, 0),
    GC32(0, 1, 1, 0, 1, 1, 1, 1),
    GC32(0, 1, 0, 1, 0, 1, 1, 1),
    GC32(1, 1, 0, 1, 0, 1, 1, 1),
    GC32(1, 0, 1, 1, 0, 0, 0, 0),
    GC32(0, 1, 1, 1, 0, 1, 1, 1),
    GC32(0, 1, 1, 1, 0, 1, 1, 1),
    GC32(0, 1, 1, 1, 0, 0, 1, 1),
    GC32(0, 1, 1, 1, 0, 0, 1, 1),
    GC32(1, 0, 1, 1, 0, 1, 1, 1),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
  },
  {
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(1, 0, 0, 0, 0, 0, 0, 0),
    GC32(1, 1, 1, 0, 0, 0, 0, 0),
    GC32(0, 1, 1, 1, 0, 0, 0, 0),
    GC32(1, 1, 1, 1, 1, 0, 0, 0),
    GC32(1, 0, 1, 1, 1, 1, 0, 0),
    GC32(1, 1, 0, 1, 1, 1, 1, 0),
    GC32(1, 1, 1, 0, 1, 1, 1, 0),
    GC32(1, 1, 1, 0, 1, 1, 1, 0),
    GC32(0, 1, 1, 1, 0, 1, 1, 1),
    GC32(0, 1, 1, 1, 0, 1, 1, 1),
    GC32(0, 1, 1, 1, 0, 1, 1, 1),
    GC32(0, 0, 0, 0, 0, 0, 1, 1),
    GC32(1, 1, 1, 1, 0, 0, 0, 0),
    GC32(1, 1, 1, 1, 0, 0, 0, 0),
    GC32(1, 1, 1, 1, 0, 0, 0, 0),
    GC32(1, 1, 1, 0, 0, 0, 0, 0),
    GC32(1, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
    GC32(0, 0, 0, 0, 0, 0, 0, 0),
  },
};


// Transform the data to serpentine to make the array look like the display
// This makes it easier to manually enter of graphical data.
#define DATR(a, b, c, d, e, f, g, h)  h,g,f,e,d,c,b,a
#define DATF(a, b, c, d, e, f, g, h)  a,b,c,d,e,f,g,h

                          
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

  int num_groups = DISPLAYCOLUMNS / 2;

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

//
// Reading image from pixel memory, shift it off to the right
//
void shiftPicture(Adafruit_NeoPixel &pixels, const GlyphColumn32 &col, const Palette &pal) {
  
  // move over the contents of the display by 1 pixel
  for(int r=0; r<DISPLAYCOLUMNS; r++) {
    shiftColumn(pixels, r);
  }
  
  //
  //  Shift in some data into the first column (vacated by the shift)
  //  It is represented as a byte instead of an array...
  //  Note we apply the mapping from the color index to the full Neo color
  //  at this point.
  //
  for(int i=0; i<PIXELSPERCOLUMN; i++) {
      //char buf[100];
      pixels.setPixelColor(NUMPIXELS-i-1, pal.color(col.row(i)));
      //sprintf(buf, "col.data() = 0x%x pal[col.row(%d)] = 0x%x", col.data(), i, pal[col.row(i)]);
      //Serial.println(buf);
  }
}

void shiftColumn(Adafruit_NeoPixel &pixels, int column) {
  
  // toss in-order column data to ooo column (and vice versa)
  int src_btm = column*8;           //-- first in column
  int dest_top = src_btm-1;         //-- last in dest column

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
  for (int i=0; i<NUMDISPLAYS; i++) {
    pixels[i].begin();
  } 

  Serial.begin(9600);
  Serial.write("Setup done!");

  if (GlyphColumn32_self_test()) {
    Serial.println("GlyphColumn32 self-test passed!");
  }

  if (!Palette_dim_test()) {
    Serial.println("Palette_dim_test failed!");
  }

  // For a set of NeoPixels the first NeoPixel is 0, second is 1
  //loadPicture(pixels, picture);
  //delay(3000);

  // This is the CS pin on the 5100 board.
  //SD.begin(4);

  // Ths is required for MEGA compatibility with the SD reader?
  //pinMode(SS, OUTPUT);

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
  Palette neo_pal("Robot1", colors, sizeof(colors)/sizeof(uint32_t));

  for (int pc=0; pc<PICTURECOLUMNS; pc++) {
    for (int d=0; d<NUMDISPLAYS; d++) {
      //char buf[100];
      //sprintf(buf, "column_data[%d][%d] = 0x%x\n", d, pc, column_data[d][pc].data());
      //Serial.print(buf);
      shiftPicture(pixels[d], column_data[d][pc], neo_pal);
      pixels[d].show();
    }
    delay(20);
  }

  /* add some padding to center the image on the display */
  for (int j=0; j<(DISPLAYCOLUMNS-PICTURECOLUMNS)/2; j++) {
    for (int d=0; d<NUMDISPLAYS; d++) {
      shiftPicture(pixels[d], GlyphColumn32(0), neo_pal);
      pixels[d].show();
    }
    delay(20);
  }

  /* wait for a time.   Of course, this is too long.  */
  delay(20000);

  // dim the palette by 1/2 and see how it changes things
  //neo_pal.dim(1);
}