#!/Library/Frameworks/Python.framework/Versions/3.6/bin/python3

from collections import defaultdict
import png

# see just how many colors get used (maybe trim the palette?)
color_freq_count = defaultdict(int)

# map to two colors for debugging
def substituteBW(value):
    if value == 0:
        return 0
    elif color_freq_count[value] > 0:
        return(1) 
    else:
        return(0)

# read in the font (into a 4-tuple)
# font_def[0] is the width of the image  (divide by 8 to get how many characters in the font)
# font_def[1] is the height of the image (divide by 8 to get how many fonts in the file)
# font_def[2] is an interator over the rows in the image
# font_def[3] is a dictionary of the meta information (palette, etc...)
#
r=png.Reader('samsho3-fonts.png')
font_def=r.read()

num_glyphs = font_def[0] // 8
palette = font_def[3]['palette']
n_colors = len(palette)

print("Palette::_colors = {")
for c in palette:
	dim = 10
	color = c[0]//dim << 16 | c[1]//dim << 8 | c[2]//dim
	print("    {},".format(hex(color)))
print("};")

img_data = list(font_def[2])

# try the second one (slice off a font's worth of data)
font = img_data[]

#
#  Rotate the font so it can shift in from the right
#  Build list of glyphs of columns (to generate code)
font_set = list()
for g in range(0,num_glyphs):
    glyph = list()
    for i in range(g*8,g*8+8):
        gcol = list()
        for j in range(0,8):
            gcol.append(font[j][i])
            # profile the color frequency so we know colors used
            color_freq_count[font[j][i]] += 1
        glyph.append(gcol)
    font_set.append(glyph)


#
#   Need to take the frequency count and come up with index substitution
#   Can do the substitution when we generate the font representation.
#
print("/* color frequency count information:")
print(color_freq_count)
print("*/")

#  Figure out how many keys there are in the dictionary and enumerate
#  them.   Use the enumerated value to create a substituion dictionary
#  where   substitute[value] = enumerated_value
#  Then, when doing the generation of the code, just do substitution

#
#  Print out the C code definition of the font
#
print("PaletteFont::glyphs = {")
letter_val = ord(' ')
for letter in font_set:
    pgm = "  FontGlyph("
    cc_count = 0
    for column in letter:
        pgm += "GC(0b"
        # This takes care of the first 7
        for value in column[0:7]:
            pgm += "{}".format(substituteBW(value))

        # special case for end of the line takes care of the last one
        if cc_count==7:
            pgm += "{})".format(substituteBW(column[7]))
        else:
            pgm += "{}),".format(substituteBW(column[7]))

        # special case to divide into multiple lines (end the line and indent)
        if cc_count==2 or cc_count == 5:
            print(pgm)
            pgm = "            "
        cc_count+=1

    pgm += "), // char {} ascii {} ".format(chr(letter_val),letter_val)
    letter_val+=1
    print(pgm)
print("};")

