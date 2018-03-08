#include <Arduino1076.h>

uint32_t default_font_color = 0x100010;

uint32_t MonoPurple_colors[2] = {
    0x000000,
    default_font_color
};


/* Names index into the Palette to extract
 * the appropriate color definition.
 * This permits brightening the palette without
 * changing names?
 */
typedef enum : unsigned byte {
	Black = 0,
	Grey  = 1,
	White = 2,
	Unk3  = 3,
	MedRed = 4,
	DkRed = 5
} NeoPaletteColorNames;

uint32_t NeoPalette_colors[NEOPALETTE_NUMCOLORS] = {
	            /*  r   g   b    ColorName   */
    0x000000,   /* {0, 0, 0}      Black      */
    0x010101,   /* {01, 01, 01}   Grey       */
    0x101010,   /* {16, 16, 16}   White      */
    0x010404,   /* {16, 4, 4}                */
    0x040000,   /* {4, 0, 0}      MedRed     */
    0x100000,   /* {16, 0, 0}     DkRed      */
    0x100400,   /* {16, 4, 0}     Amber      */
    0x101000,   /* {16, 16, 0}    Yellow?    */
    0x041000,   /* {4, 16, 0}                */
    0x001600,   /* {0, 16, 0}     DkGreen    */
    0x001004,   /* {0, 16, 16}               */
    0x000004,   /* {0, 0 , 4}     LtBlue     */
    0x040010,   /* {4, 0, 16}                */
    0x040004,   /* {4, 0, 4}      MedPurple  */
    0x100004,   /* {16, 0, 4}                */
    0x100010    /* {16, 0, 16}    DkPurple   */
};

boolean assert(char *str_expr, boolean expr) {
  if (!expr) {
    Serial.println(str_expr);
    return false;
  } else {
    return true;
  }
}
