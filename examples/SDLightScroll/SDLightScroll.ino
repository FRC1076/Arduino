// GlyphLights
// Render bit pictures in 8x32 Neopixel matrix and
// render shifting text.  (with bit font definitions)
#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>
#include <Arduino1076.h>
#include <PaletteFont.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

//#include <SPI.h>
//#include <SD.h>

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define NEODATA             6


// How many NeoPixels are attached to the Arduino?
#define NUMPANELS           3
#define NUMPIXELS         (256*NUMPANELS)
#define PIXELSPERCOLUMN     8
#define COLUMNSPERGLYPH     8
#define DISPLAYCOLUMNS   (NUMPIXELS/8)



// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels[] =  {
    Adafruit_NeoPixel(NUMPIXELS, NEODATA, NEO_GRB + NEO_KHZ800)
    // Adafruit_NeoPixel(NUMPIXELS, PIN1, NEO_GRB + NEO_KHZ800)
};

/*  static uint32_t colors[] = {
    0x0,
    0x101010,
    0x100404,
    0x001000,
    0x001004,
    0x000004,
    0x040010,
    0x041000,
    0x040004,
    0x100004,
    0x100010,
    0x010101,
    0x040000,
    0x100000,
    0x100400,
    0x101000,
};  */


static uint32_t colors[] = {
        0x0,
        0x160022,
        0x080012,
        0x000000,
        0x100010,
        0x080804,
        0x080008,
        0x040004,
        0x040404,
        0x220000
};


#define NUMCOLORS      (sizeof(NeoPalette_colors)/sizeof(uint32_t))

Palette palette(String('Neo'), MonoPurple_colors, NUMCOLORS);
PaletteFont font(String('Samurai3'), SAMURAIFONT_BASE_INDEX);

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
                           
/* move every pixel over by one, shifting the entire */
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
// Shift image over, and shift in the data in the specified GlyphColumn32 using specified palette.
//
void shiftPicture(Adafruit_NeoPixel &pixels, const GlyphColumn32 &shift_in_column, const Palette &pal) {
  for(int r=0; r<DISPLAYCOLUMNS; r++) {
    shiftColumn(pixels, r);
  }
  //
  //  Shift in some data into the first column (vacated by the shift)
  //  It is represented as a byte instead of an array...
  //
  for(int i=0; i<PIXELSPERCOLUMN; i++) {
      pixels.setPixelColor(NUMPIXELS-i-1, pal.color(shift_in_column.row(i)));
  }

  pixels.show();
}

//
// Shift image over, and shift in the data in the specified GlyphColumn32 using specified palette.
//
void shiftPicture(Adafruit_NeoPixel &pixels, byte color_index, const Palette &pal) {
  for(int r=0; r<DISPLAYCOLUMNS; r++) {
    shiftColumn(pixels, r);
  }
  //
  //  Shift in some data into the first column (vacated by the shift)
  //  It is represented as a byte instead of an array...
  //
  for(int i=0; i<PIXELSPERCOLUMN; i++) {
      pixels.setPixelColor(NUMPIXELS-i-1, pal.color(color_index));
  }

  pixels.show();
}

File JARGON;

void setup() {
  pixels[0].begin(); // This initializes the NeoPixel library.

  JARGON = SD.open("JARGON.TXT");

  Serial.begin(9600);

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  JARGON = SD.open("JARGON.TXT");

  // if the file opened okay, write to it:
  if (JARGON) {
    Serial.print("Opened Jargon.txt");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Jargon.txt");
  }


  // For a set of NeoPixels the first NeoPixel is 0, second is 1
  //loadPicture(pixels, picture);
  //delay(3000);

  // This is the CS pin on the 5100 board.
  // SD.begin(4);

  // Ths is required for MEGA compatibility with the SD reader?
  pinMode(SS, OUTPUT);

  
}

//
//   Shift in the message  (font offset is ' ' == 0)
//
void shiftInMessage(Adafruit_NeoPixel &pixels, const Palette &pal, char *message) {
  for (char *m=message; *m != NULL; m++) {
      FontGlyph32 *glyph = font.glyph(*m);

      // shift in the glyph one column at a time
      for (int col=0; col<COLUMNSPERGLYPH; col++) {
        //char buf[32];
        //sprintf(buf, "%c[%d]: 0x%x", m, col, glyph->column(col).data());
        //Serial.println(buf);
        shiftPicture(pixels, glyph->column(col), pal);
        delay(10);
      }
      // Put in a spacer between each letter
      //shiftPicture(pixels, GlyphColumn32(0), pal);
    }
}

void loop() {

  /* 
  char *nyan_top = "\x17\x18\x19\x1A";
  char *nyan_btm = "\x1B\x1C\x1D\x1E";
  for (byte ci=0; ci<16; ci++) {
    for (int lines=0; lines<2; lines++) {
      shiftPicture(pixels, ci, palette);
    }
  }
  */

#define NUMCHARS_PER_READ 16
  char Item[NUMCHARS_PER_READ + 1];
  while(JARGON.available()) {
      JARGON.read(Item, NUMCHARS_PER_READ);
      Item[NUMCHARS_PER_READ] = '\0';
      Serial.write(Item);
      shiftInMessage(pixels[0], palette, Item);
      //shiftInMessage(pixels[1], palette, nyan_btm);
      // delay(10);
    // delay(10);
  }
  JARGON.close();
  JARGON = SD.open("JARGON.TXT");
  if (JARGON) {
    Serial.print("Re-opened Jargon.txt");
  } else {
    // if the file didn't open, print an error:
    Serial.println("Failed to reopen Jargon.txt");
  }

}
