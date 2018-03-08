#!/Library/Frameworks/Python.framework/Versions/3.6/bin/python3

from collections import defaultdict
import png

# see just how many colors get used (maybe trim the palette?)
color_freq_count = defaultdict(int)

class Palette
    def __init__():
	_color_index[0] = [0]
	_num_colors = 1

    #
    # see if there is a python array search to look
    # for index of matching color
    #
    def color_index(planes):
	color = (planes[0]//10)<<12 | (planes[1]//10)<<8 | planes[2]//10
	for i in range(0,_num_colors):
	    if color == _color_index[i]:
		return i
	# if we got here, then there was not a matching
	# color in the palette.   Increment and add

# read in the font (into a 4-tuple)
# font_def[0] is the width of the image  (divide by 8 to get how many characters in the font)
# font_def[1] is the height of the image (divide by 8 to get how many fonts in the file)
# font_def[2] is an interator over the rows in the image
# font_def[3] is a dictionary of the meta information (palette, etc...)
#
r=png.Reader('robot1.png')
img_def=r.read()

n_columns = img_def[0]
n_rows = img_def[1]
column_pixels = n_columns // img_def[3]['planes']
image = img_def[2]

data_rows = []
data_columns = []
for row in range(0, n_rows):
    columns = image[row]
    for pix in range(0, column_pixels):
        plane = columns[pix*4:pix*4+4]
	
n_colors = 1 << img_def[3]['planes']     #  maps 1 << 4 == 16 colors, for example

print("Palette::_colors = {")
for c in range(0,palette._num_colors):
	color = palette._colors[c]
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

