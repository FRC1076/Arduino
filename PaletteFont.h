#ifndef PALETTEFONT_H
#define PALETTEFONT_H
//
//   Represent the bits of a letter and
//   provides right to left column iteration
//   for each of left-to-right letter animation.
//
//
//   Note, we need Arduino.h because it defines
//   the basic types we use, like byte, for example.
//
#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

class Palette {
	String _name;
	byte _size;          /* let us limit the palette to 2 colors */
	uint32_t *_palette;  /* initialized to colors array manually */
public:
	Palette(String name);
	String name(void);
	uint32_t color(byte index);  // color by color index from the palette
};

//
//  Let's stick with 16 color palettes (working from reductions)
//  This permits the GlyphColumn to represent each column with only
//  8 nibbles.
//
//#define G_C(a,b,c,d,e,f,g,h)  GlyphColumn((a|b<<8),(c|d<<8),(e|f<<8),(g|h<<8))

#define COLOR_BITS  4
#define COLOR_DEPTH (COLOR_BITS<<1)
#define FONT_WIDTH  8
#define FONT_HEIGHT 8

class GlyphColumn {
public:
	//GlyphColumn(byte r01, byte r23, byte r45, byte r67);
	GlyphColumn(byte mono_bits);     // set up for single bit BW (fits in single byte)
	GlyphColumn(const GlyphColumn &gc);
	GlyphColumn(void);
	byte row(byte row_index);        // return the nibble 0 to 7 for this column
	byte data(void);
private:
	byte _font_data;   // single bit per pixel (BW) makes one byte
};

//
//  A glyph consists of 8 columns of pixel/color data
//
class FontGlyph {
public:
	FontGlyph(GlyphColumn c0,
			  GlyphColumn c1,
			  GlyphColumn c2,
			  GlyphColumn c3,
			  GlyphColumn c4,
			  GlyphColumn c5,
			  GlyphColumn c6,
			  GlyphColumn c7);
	GlyphColumn *column(byte index);
private:
	GlyphColumn _columns[FONT_WIDTH];
};


class PaletteFont {
  public:
    PaletteFont(String name);       // instantiate the Font 
    uint8_t size();
    byte bitColumn(byte index);
    byte *byteColumn(byte index);     //  return the 8 byte array for the indexed column of the glyph
    FontGlyph *glyph(byte letter);    //
    //
    //   Start right to left column iteration and
    //   return the first(last) column.
    //   This makes it relatively convenient to
    //   to use in this way:
    //
    //   for (byte *each_column = letter.first_column();
    //        each_column != NULL;
    //        each_column = letter.next_column()) {
    //           /* do something with each_column */
    //   }
    //
    //
  private:
  	String _name;
    FontGlyph *_glyphs;      // points to Font def
    char  _letter;           // what letter is this? (not really needed)
    FontGlyph *_glyph;       // font part associated with the _letter
    byte *_columns;          // array of font columns
    byte *_column_iter;      // used for right to left iteration
    uint8_t _size;           // how many columns in the font
};

#endif