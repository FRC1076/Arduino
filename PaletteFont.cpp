#include <Adafruit_NeoPixel.h>
#include <Arduino1076.h>
#include <PaletteFont.h>
//
// Constant: font8x8_1076
// Contains an 8x8 font map for unicode points U+0020 - U+007F
// Font is rotated to be more easily shifted into 8 bit high NeoPixel
// display 1 column at a time.
//
// Font is rotation of that published by:
// And subsequently mapped to font8x8_basic by:
// black and purple for now

Palette::Palette(String name, uint32_t *colors=NULL, byte num_colors=5) : _name(name) {
  if (colors == NULL) {
    _palette = MonoPurple;
	  _size = sizeof(MonoPurple) / sizeof(uint32_t);
  } else {
    _palette = colors;
    _size = num_colors;
  }
}

uint32_t
Palette::color(byte index) const {
	if (index < 0 || index >= _size) {
		return 0x000100;           // ghost color (very dim green)
	} else {
		return _palette[index];
	}
}

String
Palette::name(void) {
	return _name;
}

byte
Palette::size(void) {
  return _size;
}

void
Palette::dim(byte factor) {
  for (int c; c<_size; c++) {
    byte r = _palette[c]>>16 & 0xff;
    byte g = _palette[c]>>8 & 0xff;
    byte b = _palette[c] & 0xff;
    r>>=factor; g>>=factor; b>>=factor;
    _palette[c] = Adafruit_NeoPixel::Color(r, g, b);
  }
}

boolean
Palette_dim_test(void) {

  uint32_t colors[] = {
        0x0,
        0x080803,
        0x090202,
        0x0c0805,
        0x0f0f0d,
  };

  Palette st_pal("Self Test", colors, sizeof(colors)/sizeof(uint32_t));
  boolean pass = true;

  pass = pass && assert("st_pal.color(0) == 0", st_pal.color(0) == 0);
  pass = pass && assert("st_pal.color(1) == 0x080803", st_pal.color(1) == 0x080803);

  st_pal.dim(1);

  pass = pass && assert("st_pal.color(1) == 0x040401", st_pal.color(1) == 0x040401);

  return pass;
}


// assemble the 0 or 1 color values into a binary byte (using preprocessor concatentation)
//#define GC(a,b,c,d,e,f,g,h) GlyphColumn((byte)a|(byte)b<<4|(byte)c<<8|(byte)d<<12|(byte)e<<16|(byte)f<<20|(byte)g<<24|(byte)h<<28)
//#define GC(a,b,c,d,e,f,g,h)  GlyphColumn(0b ## h ## g ## f ## e ## d ## c ## b ## a)
#define GC(binary_byte)    GlyphColumn(binary_byte)

// assemble the 16 color values (4 bits each) into a uint32_t (4*8==32!)
// #define GC(a,b,c,d,e,f,g,h) GlyphCol(a|b<<4|c<<8|d<<12|e<<16|f<<20|g<<24|h<<28)

// single byte can represent a column of two color values
GlyphColumn::GlyphColumn(byte column) : _font_data(column) { }

GlyphColumn::GlyphColumn(const GlyphColumn &gc) : _font_data(gc._font_data) { }

GlyphColumn::GlyphColumn(void) : _font_data(0) { }

byte
GlyphColumn::data(void) {
	return _font_data;
}


//
//  Provide means to individually extract nibble values
//  (8 values encoded in a single uint32_t)
//
byte
GlyphColumn32::row(byte row_index) const {
	if (row_index > 8) {
		return 0;
	}
	// shift and mask lower 4 bits
	byte shift = 4 * (7 - row_index);  /* (0=>28, 1=>24 ... 7=>0)  */
	return (byte)((_font_data >> shift) & 0xf);
}

// single byte can represent a column of two color values
GlyphColumn32::GlyphColumn32(uint32_t column) : _font_data(column) { }

GlyphColumn32::GlyphColumn32(const GlyphColumn32 &gc) : _font_data(gc._font_data) { }

GlyphColumn32::GlyphColumn32(void) : _font_data(0) { }

uint32_t
GlyphColumn32::data(void) {
	return _font_data;
}

GlyphColumn32 &
GlyphColumn32::operator=(const GlyphColumn32 &lhs) {
	this->_font_data = lhs._font_data;
}


boolean
GlyphColumn32_self_test(void) {

  GlyphColumn32 gc = GC32(0,1,2,3,4,5,6,7);
  boolean pass = true;

  for (int i=0; i<8; i++) {
    char buf[100];
    if (gc.row(i) != i) {
      sprintf(buf, "In gc.data() = 0x%lx gc.row(%d) != %d\n", gc.data(), i, i);
      Serial.println(buf);
      pass = false;
    }
  }

  return pass;
}


//
//  If the column_index is a byte, then we assume we
//  are doing BW image and so need to extract the info
//  from a single bit in the byte.  (derived from the column_index)
//
byte
GlyphColumn::row(byte row_index) const {
	if (row_index > 8) {
		return 0;
	}
	byte mask = 1 << row_index;   // generate the mask bit
	if (mask & _font_data) {
		return 1;
	} else {
		return 0;
	}
}

FontGlyph::FontGlyph(const GlyphColumn &c0,
					 const GlyphColumn &c1,
					 const GlyphColumn &c2,
					 const GlyphColumn &c3,
					 const GlyphColumn &c4,
					 const GlyphColumn &c5,
					 const GlyphColumn &c6,
					 const GlyphColumn &c7) {
	_columns[0] = c0;
	_columns[1] = c1;
	_columns[2] = c2;
	_columns[3] = c3;
	_columns[4] = c4;
	_columns[5] = c5;
	_columns[6] = c6;
	_columns[7] = c7;	
}

GlyphColumn &
FontGlyph::column(byte index) {
	return _columns[index];
}

FontGlyph Samurai_glyphs[] = {
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char   ascii 32 
  FontGlyph(GC(0b00000000),GC(0b00000111),GC(0b11111111),
            GC(0b11111110),GC(0b11100000),GC(0b10000000),
            GC(0b00000000),GC(0b00000000)), // char ! ascii 33 
  FontGlyph(GC(0b11000000),GC(0b11110000),GC(0b01110000),
            GC(0b01110000),GC(0b11000000),GC(0b11110000),
            GC(0b11110000),GC(0b01110000)), // char " ascii 34 
  FontGlyph(GC(0b00000000),GC(0b00111100),GC(0b01111111),
            GC(0b01111111),GC(0b00111100),GC(0b01111111),
            GC(0b01111111),GC(0b00111100)), // char # ascii 35 
  FontGlyph(GC(0b01111110),GC(0b01111110),GC(0b01111110),
            GC(0b11111111),GC(0b11111110),GC(0b01111110),
            GC(0b01111110),GC(0b00000000)), // char $ ascii 36 
  FontGlyph(GC(0b11110011),GC(0b11110110),GC(0b11111100),
            GC(0b11111100),GC(0b11111111),GC(0b11111111),
            GC(0b11101111),GC(0b01000111)), // char % ascii 37 
  FontGlyph(GC(0b00000110),GC(0b01111111),GC(0b11111111),
            GC(0b11111011),GC(0b11111111),GC(0b01101110),
            GC(0b00011111),GC(0b00011011)), // char & ascii 38 
  FontGlyph(GC(0b11000000),GC(0b11110000),GC(0b11110000),
            GC(0b01110000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char ' ascii 39 
  FontGlyph(GC(0b11111110),GC(0b11111110),GC(0b11111110),
            GC(0b11111111),GC(0b01111111),GC(0b01111111),
            GC(0b00111110),GC(0b00011100)), // char ( ascii 40 
  FontGlyph(GC(0b00001000),GC(0b00011100),GC(0b00111110),
            GC(0b01111111),GC(0b01111111),GC(0b00011100),
            GC(0b00011100),GC(0b00011100)), // char ) ascii 41 
  FontGlyph(GC(0b01100011),GC(0b11110110),GC(0b11111100),
            GC(0b00111100),GC(0b00111110),GC(0b11111111),
            GC(0b11101110),GC(0b01000100)), // char * ascii 42 
  FontGlyph(GC(0b00011000),GC(0b00011000),GC(0b11111111),
            GC(0b11111110),GC(0b11111100),GC(0b01011000),
            GC(0b00011000),GC(0b00000000)), // char + ascii 43 
  FontGlyph(GC(0b00000000),GC(0b00000110),GC(0b00000111),
            GC(0b00000111),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char , ascii 44 
  FontGlyph(GC(0b00000000),GC(0b00011100),GC(0b00011100),
            GC(0b00011100),GC(0b00011000),GC(0b00011000),
            GC(0b00010000),GC(0b00000000)), // char - ascii 45 
  FontGlyph(GC(0b00000111),GC(0b00000111),GC(0b00000111),
            GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char . ascii 46 
  FontGlyph(GC(0b00000000),GC(0b00000011),GC(0b00001110),
            GC(0b00111100),GC(0b01111000),GC(0b11110000),
            GC(0b11100000),GC(0b01000000)), // char / ascii 47 
  FontGlyph(GC(0b00111110),GC(0b01111111),GC(0b11100111),
            GC(0b11000011),GC(0b11000111),GC(0b11111110),
            GC(0b01111100),GC(0b00111000)), // char 0 ascii 48 
  FontGlyph(GC(0b00000110),GC(0b01100111),GC(0b01111111),
            GC(0b11111110),GC(0b11111110),GC(0b11000110),
            GC(0b00000100),GC(0b00000000)), // char 1 ascii 49 
  FontGlyph(GC(0b01110110),GC(0b11110111),GC(0b11101111),
            GC(0b11011111),GC(0b11111110),GC(0b11110110),
            GC(0b11110110),GC(0b01100110)), // char 2 ascii 50 
  FontGlyph(GC(0b11000110),GC(0b11100110),GC(0b11100110),
            GC(0b11111111),GC(0b11111111),GC(0b11111111),
            GC(0b11101110),GC(0b01000100)), // char 3 ascii 51 
  FontGlyph(GC(0b00001110),GC(0b00011110),GC(0b00111100),
            GC(0b11111100),GC(0b11111111),GC(0b11111110),
            GC(0b01111100),GC(0b01001000)), // char 4 ascii 52 
  FontGlyph(GC(0b01100110),GC(0b11111110),GC(0b11111111),
            GC(0b11111111),GC(0b11111111),GC(0b11011111),
            GC(0b11011110),GC(0b10001100)), // char 5 ascii 53 
  FontGlyph(GC(0b00111110),GC(0b01111111),GC(0b11111111),
            GC(0b11011011),GC(0b11011111),GC(0b11111110),
            GC(0b11111110),GC(0b01101100)), // char 6 ascii 54 
  FontGlyph(GC(0b11100000),GC(0b11110000),GC(0b01110110),
            GC(0b01101111),GC(0b11111111),GC(0b11111011),
            GC(0b11100000),GC(0b01000000)), // char 7 ascii 55 
  FontGlyph(GC(0b00111110),GC(0b01111111),GC(0b11111111),
            GC(0b11011011),GC(0b11011011),GC(0b11111110),
            GC(0b11111100),GC(0b01011000)), // char 8 ascii 56 
  FontGlyph(GC(0b01110110),GC(0b11111111),GC(0b11111111),
            GC(0b11011011),GC(0b11011111),GC(0b11111110),
            GC(0b11111100),GC(0b01111000)), // char 9 ascii 57 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b01110111),GC(0b01110111),GC(0b01110111),
            GC(0b00000000),GC(0b00000000)), // char : ascii 58 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b01111110),GC(0b01111111),GC(0b01101111),
            GC(0b00000000),GC(0b00000000)), // char ; ascii 59 
  FontGlyph(GC(0b00000000),GC(0b00010000),GC(0b00111000),
            GC(0b01111111),GC(0b11111111),GC(0b11100111),
            GC(0b01000011),GC(0b00000000)), // char < ascii 60 
  FontGlyph(GC(0b01101100),GC(0b01111110),GC(0b01111110),
            GC(0b01111110),GC(0b01111110),GC(0b01101100),
            GC(0b01101100),GC(0b01001000)), // char = ascii 61 
  FontGlyph(GC(0b00000000),GC(0b11000011),GC(0b11100111),
            GC(0b11111111),GC(0b11111110),GC(0b01111000),
            GC(0b00010000),GC(0b00000000)), // char > ascii 62 
  FontGlyph(GC(0b11100000),GC(0b11100000),GC(0b11111111),
            GC(0b11111111),GC(0b11111111),GC(0b11111000),
            GC(0b11110000),GC(0b01100000)), // char ? ascii 63 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char @ ascii 64 
  FontGlyph(GC(0b00001110),GC(0b00001111),GC(0b00111111),
            GC(0b11111111),GC(0b11111100),GC(0b11111100),
            GC(0b00111111),GC(0b00001011)), // char A ascii 65 
  FontGlyph(GC(0b11100011),GC(0b11111111),GC(0b11111111),
            GC(0b11011011),GC(0b11111111),GC(0b11111111),
            GC(0b11111110),GC(0b01101100)), // char B ascii 66 
  FontGlyph(GC(0b01111100),GC(0b11111110),GC(0b11001111),
            GC(0b11000111),GC(0b11000111),GC(0b11100111),
            GC(0b01101110),GC(0b00001100)), // char C ascii 67 
  FontGlyph(GC(0b11000110),GC(0b11111111),GC(0b11111111),
            GC(0b11000111),GC(0b11001110),GC(0b11111110),
            GC(0b11111100),GC(0b01111000)), // char D ascii 68 
  FontGlyph(GC(0b11100011),GC(0b11111111),GC(0b11111111),
            GC(0b11111111),GC(0b11111111),GC(0b11011111),
            GC(0b11010111),GC(0b10000100)), // char E ascii 69 
  FontGlyph(GC(0b11000011),GC(0b11111111),GC(0b11111110),
            GC(0b11111000),GC(0b11111000),GC(0b11111000),
            GC(0b11010000),GC(0b10000000)), // char F ascii 70 
  FontGlyph(GC(0b00111110),GC(0b01111111),GC(0b11111111),
            GC(0b11011111),GC(0b11011110),GC(0b11111111),
            GC(0b11111111),GC(0b01010011)), // char G ascii 71 
  FontGlyph(GC(0b01111000),GC(0b11111111),GC(0b11111111),
            GC(0b00111000),GC(0b00111000),GC(0b11111110),
            GC(0b11111111),GC(0b11111111)), // char H ascii 72 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b01111111),
            GC(0b11111111),GC(0b11111110),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char I ascii 73 
  FontGlyph(GC(0b00011110),GC(0b00011111),GC(0b00011011),
            GC(0b11000111),GC(0b11111111),GC(0b11111110),
            GC(0b11111100),GC(0b01000000)), // char J ascii 74 
  FontGlyph(GC(0b11000011),GC(0b11111111),GC(0b11111111),
            GC(0b11111111),GC(0b01101110),GC(0b11100111),
            GC(0b11000111),GC(0b10000011)), // char K ascii 75 
  FontGlyph(GC(0b11000110),GC(0b11111111),GC(0b11111111),
            GC(0b01100110),GC(0b00001110),GC(0b00001110),
            GC(0b00001110),GC(0b00000110)), // char L ascii 76 
  FontGlyph(GC(0b01111000),GC(0b11111111),GC(0b11111111),
            GC(0b00111110),GC(0b00111100),GC(0b11111111),
            GC(0b11111111),GC(0b01111111)), // char M ascii 77 
  FontGlyph(GC(0b01110011),GC(0b11111111),GC(0b11111111),
            GC(0b01111100),GC(0b00011110),GC(0b11111111),
            GC(0b11111111),GC(0b01001110)), // char N ascii 78 
  FontGlyph(GC(0b00111100),GC(0b01111110),GC(0b11001110),
            GC(0b11000111),GC(0b11000111),GC(0b11101111),
            GC(0b01111110),GC(0b00111100)), // char O ascii 79 
  FontGlyph(GC(0b11000000),GC(0b11111111),GC(0b11111111),
            GC(0b11101100),GC(0b11111100),GC(0b11111000),
            GC(0b11110000),GC(0b01100000)), // char P ascii 80 
  FontGlyph(GC(0b01111100),GC(0b11111110),GC(0b11001111),
            GC(0b11011111),GC(0b11011110),GC(0b11011111),
            GC(0b01111111),GC(0b00110111)), // char Q ascii 81 
  FontGlyph(GC(0b11001110),GC(0b11111111),GC(0b11111111),
            GC(0b11101100),GC(0b11111110),GC(0b11111111),
            GC(0b11110111),GC(0b01100011)), // char R ascii 82 
  FontGlyph(GC(0b01110110),GC(0b11111111),GC(0b11111011),
            GC(0b11011011),GC(0b11011111),GC(0b11111111),
            GC(0b11111110),GC(0b01101100)), // char S ascii 83 
  FontGlyph(GC(0b11000000),GC(0b11100000),GC(0b11100000),
            GC(0b11111111),GC(0b11111111),GC(0b11000000),
            GC(0b11000000),GC(0b10000000)), // char T ascii 84 
  FontGlyph(GC(0b01111110),GC(0b11111111),GC(0b11000111),
            GC(0b00000011),GC(0b01100111),GC(0b11111110),
            GC(0b11111100),GC(0b01111000)), // char U ascii 85 
  FontGlyph(GC(0b11000000),GC(0b11111000),GC(0b11111111),
            GC(0b01111111),GC(0b00111111),GC(0b11111100),
            GC(0b11110000),GC(0b01000000)), // char V ascii 86 
  FontGlyph(GC(0b11000000),GC(0b11111110),GC(0b11111111),
            GC(0b01011110),GC(0b11011110),GC(0b11111111),
            GC(0b01111110),GC(0b01100000)), // char W ascii 87 
  FontGlyph(GC(0b01100011),GC(0b11110111),GC(0b11111110),
            GC(0b00111100),GC(0b00111110),GC(0b11111111),
            GC(0b11100110),GC(0b01000100)), // char X ascii 88 
  FontGlyph(GC(0b01100000),GC(0b11110000),GC(0b11111000),
            GC(0b01111111),GC(0b00111111),GC(0b11110110),
            GC(0b11000000),GC(0b10000000)), // char Y ascii 89 
  FontGlyph(GC(0b00000110),GC(0b11000111),GC(0b11001111),
            GC(0b11101111),GC(0b11111110),GC(0b11110110),
            GC(0b11100110),GC(0b11000110)), // char Z ascii 90 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b01111110),
            GC(0b11111111),GC(0b11111111),GC(0b11100111),
            GC(0b11100111),GC(0b00000000)), // char [ ascii 91 
  FontGlyph(GC(0b00000000),GC(0b11111100),GC(0b11111100),
            GC(0b11111111),GC(0b00111111),GC(0b11111110),
            GC(0b11111100),GC(0b10111100)), // char \ ascii 92 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b11100111),
            GC(0b11100111),GC(0b11111111),GC(0b11111110),
            GC(0b01111100),GC(0b00000000)), // char ] ascii 93 
  FontGlyph(GC(0b00000000),GC(0b00000011),GC(0b00000011),
            GC(0b00000011),GC(0b00000011),GC(0b00000011),
            GC(0b00000011),GC(0b00000000)), // char ^ ascii 94 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char _ ascii 95 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char ` ascii 96 
  FontGlyph(GC(0b00001110),GC(0b00001111),GC(0b00111111),
            GC(0b11111111),GC(0b11111100),GC(0b11111100),
            GC(0b00111111),GC(0b00001011)), // char a ascii 97 
  FontGlyph(GC(0b11100011),GC(0b11111111),GC(0b11111111),
            GC(0b11011011),GC(0b11111111),GC(0b11111111),
            GC(0b11111110),GC(0b01101100)), // char b ascii 98 
  FontGlyph(GC(0b01111100),GC(0b11111110),GC(0b11001111),
            GC(0b11000111),GC(0b11000111),GC(0b11100111),
            GC(0b01101110),GC(0b00001100)), // char c ascii 99 
  FontGlyph(GC(0b11000110),GC(0b11111111),GC(0b11111111),
            GC(0b11000111),GC(0b11001110),GC(0b11111110),
            GC(0b11111100),GC(0b01111000)), // char d ascii 100 
  FontGlyph(GC(0b11100011),GC(0b11111111),GC(0b11111111),
            GC(0b11111111),GC(0b11111111),GC(0b11011111),
            GC(0b11010111),GC(0b10000100)), // char e ascii 101 
  FontGlyph(GC(0b11000011),GC(0b11111111),GC(0b11111110),
            GC(0b11111000),GC(0b11111000),GC(0b11111000),
            GC(0b11010000),GC(0b10000000)), // char f ascii 102 
  FontGlyph(GC(0b00111110),GC(0b01111111),GC(0b11111111),
            GC(0b11011111),GC(0b11011110),GC(0b11111111),
            GC(0b11111111),GC(0b01010011)), // char g ascii 103 
  FontGlyph(GC(0b01111000),GC(0b11111111),GC(0b11111111),
            GC(0b00111000),GC(0b00111000),GC(0b11111110),
            GC(0b11111111),GC(0b11111111)), // char h ascii 104 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b01111111),
            GC(0b11111111),GC(0b11111110),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char i ascii 105 
  FontGlyph(GC(0b00011110),GC(0b00011111),GC(0b00011011),
            GC(0b11000111),GC(0b11111111),GC(0b11111110),
            GC(0b11111100),GC(0b01000000)), // char j ascii 106 
  FontGlyph(GC(0b11000011),GC(0b11111111),GC(0b11111111),
            GC(0b11111111),GC(0b01101110),GC(0b11100111),
            GC(0b11000111),GC(0b10000011)), // char k ascii 107 
  FontGlyph(GC(0b11000110),GC(0b11111111),GC(0b11111111),
            GC(0b01100110),GC(0b00001110),GC(0b00001110),
            GC(0b00001110),GC(0b00000110)), // char l ascii 108 
  FontGlyph(GC(0b01111000),GC(0b11111111),GC(0b11111111),
            GC(0b00111110),GC(0b00111100),GC(0b11111111),
            GC(0b11111111),GC(0b01111111)), // char m ascii 109 
  FontGlyph(GC(0b01110011),GC(0b11111111),GC(0b11111111),
            GC(0b01111100),GC(0b00011110),GC(0b11111111),
            GC(0b11111111),GC(0b01001110)), // char n ascii 110 
  FontGlyph(GC(0b00111100),GC(0b01111110),GC(0b11001110),
            GC(0b11000111),GC(0b11000111),GC(0b11101111),
            GC(0b01111110),GC(0b00111100)), // char o ascii 111 
  FontGlyph(GC(0b11000000),GC(0b11111111),GC(0b11111111),
            GC(0b11101100),GC(0b11111100),GC(0b11111000),
            GC(0b11110000),GC(0b01100000)), // char p ascii 112 
  FontGlyph(GC(0b01111100),GC(0b11111110),GC(0b11001111),
            GC(0b11011111),GC(0b11011110),GC(0b11011111),
            GC(0b01111111),GC(0b00110111)), // char q ascii 113 
  FontGlyph(GC(0b11001110),GC(0b11111111),GC(0b11111111),
            GC(0b11101100),GC(0b11111110),GC(0b11111111),
            GC(0b11110111),GC(0b01100011)), // char r ascii 114 
  FontGlyph(GC(0b01110110),GC(0b11111111),GC(0b11111011),
            GC(0b11011011),GC(0b11011111),GC(0b11111111),
            GC(0b11111110),GC(0b01101100)), // char s ascii 115 
  FontGlyph(GC(0b11000000),GC(0b11100000),GC(0b11100000),
            GC(0b11111111),GC(0b11111111),GC(0b11000000),
            GC(0b11000000),GC(0b10000000)), // char t ascii 116 
  FontGlyph(GC(0b01111110),GC(0b11111111),GC(0b11000111),
            GC(0b00000011),GC(0b01100111),GC(0b11111110),
            GC(0b11111100),GC(0b01111000)), // char u ascii 117 
  FontGlyph(GC(0b11000000),GC(0b11111000),GC(0b11111111),
            GC(0b01111111),GC(0b00111111),GC(0b11111100),
            GC(0b11110000),GC(0b01000000)), // char v ascii 118 
  FontGlyph(GC(0b11000000),GC(0b11111110),GC(0b11111111),
            GC(0b01011110),GC(0b11011110),GC(0b11111111),
            GC(0b01111110),GC(0b01100000)), // char w ascii 119 
  FontGlyph(GC(0b01100011),GC(0b11110111),GC(0b11111110),
            GC(0b00111100),GC(0b00111110),GC(0b11111111),
            GC(0b11100110),GC(0b01000100)), // char x ascii 120 
  FontGlyph(GC(0b01100000),GC(0b11110000),GC(0b11111000),
            GC(0b01111111),GC(0b00111111),GC(0b11110110),
            GC(0b11000000),GC(0b10000000)), // char y ascii 121 
  FontGlyph(GC(0b00000110),GC(0b11000111),GC(0b11001111),
            GC(0b11101111),GC(0b11111110),GC(0b11110110),
            GC(0b11100110),GC(0b11000110)), // char z ascii 122 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char { ascii 123 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char | ascii 124 
  FontGlyph(GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000),GC(0b00000000),
            GC(0b00000000),GC(0b00000000)), // char } ascii 125 
  FontGlyph(GC(0b01010101),GC(0b01010101),GC(0b01010101),
            GC(0b01010101),GC(0b01010101),GC(0b01010101),
            GC(0b01010101),GC(0b01010101)) // char ~ ascii 126  
};


// This could, perhaps be replaced with something that reads
// the appropriate glyph def from SD card, thus saving us tons
// of memory.
PaletteFont::PaletteFont(String name) : _name(name), _glyphs(Samurai_glyphs) {
	_size = sizeof(Samurai_glyphs) / sizeof(FontGlyph);
}

uint8_t
PaletteFont::size() {
	return _size;
}

FontGlyph *
PaletteFont::glyph(byte index) {
	return _glyphs+index;
}

/* access the columns by index */
byte
PaletteFont::bitColumn(byte index) {
	return _columns[index];
}

/* access the column as an array of pallete color indices */
byte *
PaletteFont::byteColumn(byte index) {
	return ((byte *)_glyph)[index*8];
}

