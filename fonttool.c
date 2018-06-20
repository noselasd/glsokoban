#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// FreeType requires this stuff to include the correct headers
#include <ft2build.h>
#include FT_FREETYPE_H



/**
 * A font file contains the following:
 * 2 chars: "F1" (Font, version 1)
 * 32 bit: Texture width
 * 32 bit: Texture height
 * 32 bit: Line height
 * 32 bit : Number of characters
 * Then Number of characters entries for each glyph:
 * 1 byte - character ascii number
 * 1 byte - character width
 * 16 bit - x texcoord
 * 16 bit - y texcoord
 * then bytes of texture width * texture height 
 * makes up the texture data
 * 16 and 32 bit numbers are stored in big endian 
 *
 */

// Every glyph/character has such a struct in the output file. This
// contains it's ascii code, the width of the character and the x 
// and y coordinates where this character can be found in the texture.
typedef struct GlyphEntry GlyphEntry;
struct GlyphEntry {
	uint8_t ascii, width;
	uint16_t x, y;
};

// Convenience function for writing simple objects to files.

static int pickle_u(FILE * f, uint32_t u)
{
	unsigned char b[4];
	size_t ret;

	b[0] = (u & 0xff000000) >> 24;
	b[1] = (u & 0x00ff0000) >> 16;
	b[2] = (u & 0x0000ff00) >> 8;
	b[3] = (u & 0x000000ff);

	ret = fwrite(b, 1, 4, f);

	return ret == 4;

}
static int pickle_s(FILE * f, uint16_t u)
{
	unsigned char b[2];
	size_t ret;

	b[0] = (u & 0xff00) >> 8;
	b[1] = (u & 0x00ff);

	ret = fwrite(b, 1, 2, f);

	return ret == 2;

}

#define CHECKRET(b) if((ret) <= 0) return (b)
static int pickle_glyphentry(FILE * f, GlyphEntry * e)
{
	size_t ret;

	ret = fwrite(&e->ascii, 1, 1, f);
	CHECKRET(0);
	ret = fwrite(&e->width, 1, 1, f);
	CHECKRET(0);
	ret = pickle_s(f, e->x);
	CHECKRET(0);
	ret = pickle_s(f, e->y);
	CHECKRET(0);

	return 1;

}

static void do_abort(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

// This function does all the work.
static void Create_Font(const char *fontfile, size_t font_size, const char *outfile)
{
	// These are the characters that get stored. The last one ('\xFF')
	// indicates the picture used to draw 'unknown' characters.
	const char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~!@#$%^&*()-=+;:'\",./?[]|\\ <>`\xFF";
	// Margins around characters to prevent them from 'bleeding' into
	// each other.
	size_t margin = 3;
	size_t image_height = 0, image_width = 256;
	size_t i;
	int max_descent = 0, max_ascent = 0;
	size_t space_on_line;
	size_t needed_image_height;
	size_t lines = 1;
	size_t x = 1,y;
	unsigned char *image;
	GlyphEntry *entries;
	FILE *out;
	int pr;
	size_t imret;
	

	// This initializes FreeType
	FT_Library library;
	if (FT_Init_FreeType(&library) != 0)
		do_abort("Could not initialize FreeType2 library.");

	// Load the font
	FT_Face face;
	if (FT_New_Face(library, fontfile, 0, &face) != 0)
		do_abort("Could not load font file.");

	// Abort if this is not a 'true type', scalable font.
	if (!(face->face_flags & FT_FACE_FLAG_SCALABLE) || !(face->face_flags & FT_FACE_FLAG_HORIZONTAL))
		do_abort("Error setting font size.");

	// Set the font size
	FT_Set_Pixel_Sizes(face, font_size, 0);

	// First we go over all the characters to find the max descent
	// and ascent (space required above and below the base of a
	// line of text) and needed image size. There are simpler methods
	// to obtain these with FreeType but they are unreliable.
	space_on_line = image_width - margin;

	for (i = 0; i < (sizeof(chars) - 1); ++i) {
		// Look up the character in the font file.
		size_t char_index = FT_Get_Char_Index(face, (unsigned int) chars[i]);
		size_t advance;
		if (chars[i] == '\xFF')
			char_index = 0;

		// Render the current glyph.
		FT_Load_Glyph(face, char_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		advance = (face->glyph->metrics.horiAdvance >> 6) + margin;
		// If the line is full go to the next line
		if (advance > space_on_line) {
			space_on_line = image_width - margin;
			++lines;
		}
		space_on_line -= advance;

		max_ascent = face->glyph->bitmap_top > max_ascent ? face->glyph->bitmap_top : max_ascent;

		max_descent = (face->glyph->bitmap.rows - face->glyph->bitmap_top) > max_descent ? (face->glyph->bitmap.rows - face->glyph->bitmap_top) : max_descent;
	}

	// Compute how high the texture has to be.
	needed_image_height = (max_ascent + max_descent + margin) * lines + margin;
	// Get the first power of two in which it fits.
	image_height = 16;
	while (image_height < needed_image_height)
		image_height *= 2;

	// Allocate memory for the texture, and set it to 0
	image = malloc(image_height * image_width);
	for (i = 0; i < image_height * image_width; ++i)
		image[i] = 0;

	// Allocate space for the GlyphEntries
	entries = malloc((sizeof(chars) - 1) * sizeof(GlyphEntry));
	// These are the position at which to draw the next glyph
	x = margin, y = margin + max_ascent;

	// Drawing loop
	for (i = 0; i < (sizeof(chars) - 1); ++i) {
		size_t char_index = FT_Get_Char_Index(face, (unsigned int) chars[i]);
		size_t advance;
		int row,pixel;
		if (chars[i] == '\xFF')
			char_index = 0;

		// Render the glyph
		FT_Load_Glyph(face, char_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		// See whether the character fits on the current line
		advance = (face->glyph->metrics.horiAdvance >> 6) + margin;
		if (advance > image_width - x) {
			x = margin;
			y += (max_ascent + max_descent + margin);
		}
		// Fill in the GlyphEntry
		entries[i].ascii = chars[i];
		entries[i].width = advance - 3;
		entries[i].x = x;
		entries[i].y = y - max_ascent;

		// Copy the image gotten from FreeType onto the texture
		// at the correct position
		for (row = 0; row < face->glyph->bitmap.rows; ++row) {
			for (pixel = 0; pixel < face->glyph->bitmap.width; ++pixel) {
				image[(x + face->glyph->bitmap_left + pixel) + (y - face->glyph->bitmap_top + row) * image_width] = face->glyph->bitmap.buffer[pixel + row * face->glyph->bitmap.pitch];
			}
		}

		x += advance;
	}

	// Write everything to the output file (see top of this
	// file for the format specification)
	out = fopen(outfile, "wb");
	if (out == NULL)
		do_abort("cannot open outfile");
	fprintf(out, "%s", "F1");
	pr = pickle_u(out, image_width);
	pr += pickle_u(out, image_height);
	pr += pickle_u(out, max_ascent + max_descent);
	pr += pickle_u(out, sizeof(chars) - 1);
	if (pr != 4)
		do_abort("Error writing font header");


	// GlyphEntries
	for (i = 0; i < (sizeof(chars) - 1); ++i) {
		int ret;
		ret = pickle_glyphentry(out, &entries[i]);
		if (!ret)
			do_abort("Error writing font glyph entries :-(");
	}
	// Texture data
	imret = fwrite(image, 1, image_height * image_width, out);
	if (imret != (image_width * image_height))
		do_abort("Error writing font data :-(");

	free(image);

	FT_Done_FreeType(library);
	printf("Wrote %s , %zu by %zu pixels\n", outfile, image_width, image_height);
}

// Main interprets the arguments and handles errors.
int main(int args, char **argv)
{
	// Default size
	size_t size = 14;

	if (args < 3) {
		printf("Usage %s ttf-font-file output file [font size]\n", argv[0]);
	} else {
		if (args > 3) {
			size_t arg_size = atoi(argv[3]);
			if (arg_size != 0)
				size = arg_size;
		}
		Create_Font(argv[1], size, argv[2]);
		return 0;
	}

	return 1;
}

//Made by Nils O. Selåsdal
