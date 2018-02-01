#ifndef FONTLETTER_H
#define FONTLETTER_H
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

extern byte font8x8_basic[128][8];

class FontLetter {
  public:
    FontLetter(char c, byte p[], uint8_t sz);
    uint8_t size();
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
    byte *first_column();
    byte *next_column();
  private:
    char  _letter;        // what letter is this? (not really needed)
    byte *_columns;       // array of font columns
    byte *_column_iter;   // used for right to left iteration
    uint8_t _size;        // how many columns in the font
};

#endif