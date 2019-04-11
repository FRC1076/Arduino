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
#define NEODATA0            6
#define NEODATA1            7

// How many NeoPixels are attached to the Arduino?
#define NUMPANELS           2
#define NUMPIXELS         (256*NUMPANELS)
#define PIXELSPERCOLUMN     8
#define COLUMNSPERGLYPH     8
#define DISPLAYCOLUMNS   (NUMPIXELS/8)

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels[] =  {
    Adafruit_NeoPixel(NUMPIXELS, NEODATA0, NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(NUMPIXELS, NEODATA1, NEO_GRB + NEO_KHZ800)
};

#define NUMCOLORS      (sizeof(NeoPalette_colors)/sizeof(uint32_t))
#define SCROLL_DELAY_MS      10

Palette palette(String('Neo'), MonoPurple_colors, NUMCOLORS);
PaletteFont font(String('Samurai3'), SAMURAIFONT_BASE_INDEX);

// Transform the data to serpentine to make the array look like the display
#define DATR(a, b, c, d, e, f, g, h)  h,g,f,e,d,c,b,a
#define DATF(a, b, c, d, e, f, g, h)  a,b,c,d,e,f,g,h

// Forward declaration due to default value
void shiftInMessage(Adafruit_NeoPixel pixels[], const Palette &pal, char *message, uint32_t delayms = SCROLL_DELAY_MS);
                     
/* move every pixel over by one, shifting the entire */
void shiftColumn(Adafruit_NeoPixel pixels[], int column) {
  
  // toss in-order column data to ooo column (and vice versa)
  int src_btm = column*8;           //-- first in column
  int dest_top = src_btm-1;         //-- last in dest column

  //
  // transfer data starting at first src element (increasing index)
  // to the location starting at first dest (decreasing index)
  // This is general enough that it works for zig or zag columns
  //
  for (int i=0; i<PIXELSPERCOLUMN; i++) {
      for (int j=0; j<2; j++) {
          uint32_t p = pixels[j].getPixelColor(src_btm+i);
          pixels[j].setPixelColor(dest_top-i, p);
      }  
  }
}

//
// Shift image over, and shift in the data in the specified GlyphColumn32 using specified palette.
//
void shiftPicture(Adafruit_NeoPixel pixels[], const GlyphColumn32 &shift_in_column, const Palette &pal) {
  for(int r=0; r<DISPLAYCOLUMNS; r++) {
      shiftColumn(pixels, r);
  }
  //
  //  Shift in some data into the first column (vacated by the shift)
  //  It is represented as a byte instead of an array...
  //
  for(int i=0; i<PIXELSPERCOLUMN; i++) {
      for (int j=0; j<2; j++) {
          pixels[j].setPixelColor(NUMPIXELS-i-1, pal.color(shift_in_column.row(i)));
      }
  }

  for (int k=0; k<2; k++) {
      pixels[k].show();
  }
}

//
// Shift image over, and shift in the data in the specified GlyphColumn32 using specified palette.
//
void shiftPicture(Adafruit_NeoPixel pixels[], byte color_index, const Palette &pal) {
  for(int r=0; r<DISPLAYCOLUMNS; r++) {
      shiftColumn(pixels, r);
  }
  //
  //  Shift in some data into the first column (vacated by the shift)
  //  It is represented as a byte instead of an array...
  //
  for(int i=0; i<PIXELSPERCOLUMN; i++) {
      for (int j=0; j<2; j++) {
          pixels[j].setPixelColor(NUMPIXELS-i-1, pal.color(color_index));
      }
  }
  for (int k=0; k<2; k++) {
      pixels[k].show();
  }
}

File JARGON;

void setup() {
  pixels[0].begin(); // This initializes the NeoPixel library.
  pixels[1].begin();

  // I think this is required for MEGA compatibility with the SD reader?
  pinMode(SS, OUTPUT);

  Serial.begin(9600);

  shiftInMessage(pixels, palette, "Initializing SD Card...");

  if (!SD.begin(4)) {
      shiftInMessage(pixels, palette, "***SD Card Init Failed***    ");
      // while (1);     Okay to continue, we get error messages on display.
  } else {
      shiftInMessage(pixels, palette, "...SD Card Initialized!    ");
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  JARGON = SD.open("JARGON.TXT");

  // if the file opened okay, write to it:
  if (JARGON) {
      shiftInMessage(pixels, palette, "Opened JARGON.TXT        ");
  } else {
      // if the file didn't open, print an error:
      shiftInMessage(pixels, palette, "**Error opening JARGON.TXT***");
  }
  
}

//
//   Shift in the message  (font offset is ' ' == 0)
//
void shiftInMessage(Adafruit_NeoPixel pixels[], const Palette &pal, char *message, uint32_t delayms) {
  for (char *m=message; *m != NULL; m++) {
      FontGlyph32 *glyph = font.glyph(*m);

      // shift in the glyph one column at a time
      for (int col=0; col<COLUMNSPERGLYPH; col++) {
          //char buf[32];
          //sprintf(buf, "%c[%d]: 0x%x", m, col, glyph->column(col).data());
          //Serial.println(buf);
          shiftPicture(pixels, glyph->column(col), pal);
          delay(delayms);
      }
      // Put in a spacer between each letter
      //shiftPicture(pixels, GlyphColumn32(0), pal);
    }
}

void loop() {

#define BUF_SIZE      1
#define END_OF_FILE  -1

  char Item[BUF_SIZE + 1];   // one character to terminate string
  //
  // For a change, read a single character, terminate it
  // This permits us to cleanly detect EOF, and promotes smooth
  // scrolling.   And we still have a string, albeit a single character
  // string to pass to the shiftInMessage.
  //
  while((Item[0] = JARGON.read()) !=  END_OF_FILE) {
      Item[1] = '\0';
      //Serial.write(Item);
      shiftInMessage(pixels, palette, Item, SCROLL_DELAY_MS);
  }
  // If we are here, we 
  JARGON.close();
  JARGON = SD.open("JARGON.TXT");
  if (JARGON) {
      shiftInMessage(pixels, palette, "...Thinking about more to say...        ");
  } else {
      // if the file didn't open, print an error:
      shiftInMessage(pixels, palette, "***That's all folks!***");
  }
}
