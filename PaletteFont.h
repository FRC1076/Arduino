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
	byte _size;                   /* let us limit the palette to 2 colors */
public:
	Palette(String name);
	String name(void);
	uint32_t *_palette;           /* initialized to colors array manually */
	uint32_t color(byte index);   // color by color index from the palette
};

//
//  Let's stick with 16 color palettes (working from reductions)
//  This permits the GlyphColumn to represent each column with only
//  8 nibbles.
//
#define GC32(a,b,c,d,e,f,g,h)  GlyphColumn32((uint32_t)((uint32_t)a)<<28 | \
													   ((uint32_t)b)<<24 | \
													   ((uint32_t)c)<<20 | \
													   ((uint32_t)d)<<16 | \
													   ((uint32_t)e)<<12 | \
													   ((uint32_t)f)<<8  | \
													   ((uint32_t)g)<<4  | \
													   ((uint32_t)h))

#define COLOR_BITS  4
#define COLOR_DEPTH (COLOR_BITS<<1)
#define FONT_WIDTH  8
#define FONT_HEIGHT 8

class GlyphColumn {
public:
	GlyphColumn(byte mono_bits);     // set up for single bit BW (fits in single byte)
	GlyphColumn(const GlyphColumn &gc);
	GlyphColumn(void);
	byte row(byte row_index) const;        // return the nibble 0 to 7 for this column
	byte data(void);
private:
	byte _font_data;   // single bit per pixel (BW) makes one byte
};

class GlyphColumn32 {
public:
	GlyphColumn32(uint32_t bits16);     // set up for 8 pixels of 16 bit paletted color
	GlyphColumn32(const GlyphColumn32 &gc);
	GlyphColumn32 &operator=(const GlyphColumn32 &gc);
	GlyphColumn32(void);
	byte row(byte row_index) const;        // return the nibble 0 to 7 for this column
	uint32_t data(void);
private:
	uint32_t _font_data;   // single bit per pixel (BW) makes one byte
};

//
//  A glyph consists of 8 columns of pixel/color data
//
class FontGlyph {
public:
	FontGlyph(const GlyphColumn &c0,
			  const GlyphColumn &c1,
			  const GlyphColumn &c2,
			  const GlyphColumn &c3,
			  const GlyphColumn &c4,
			  const GlyphColumn &c5,
			  const GlyphColumn &c6,
			  const GlyphColumn &c7);
	GlyphColumn &column(byte index);
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