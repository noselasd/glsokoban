#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "common.h"

#define MAINFONT "LucidaSR.font"

struct Glyph {
	float tex_x1, tex_y1, tex_x2;
	uint32_t advance;
};

typedef struct FTFont FTFont;
struct FTFont {

	struct Glyph *glyphs;
	struct Glyph *table[256];

	uint32_t line_height;
    unsigned int texid;
	float tex_line_height;
};

struct Glyph_Buffer {
	uint8_t ascii, width;
	uint16_t x, y;
};

static FTFont *mainfont;

int pick_u(FILE * f, uint32_t * out)
{
	unsigned char buf[4];
	if (fread(buf, 1, 4, f) != 4)
		return 0;

	*out = buf[0] << 24;
	*out |= buf[1] << 16;
	*out |= buf[2] << 8;
	*out |= buf[3];

	return 1;
}

int pick_s(FILE * f, uint16_t * out)
{
	unsigned char buf[2];
	if (fread(buf, 1, 2, f) != 2)
		return 0;

	*out = buf[0] << 8;
	*out |= buf[1];

	return 1;
}

static int pick_glyphbuffer(FILE * f, struct Glyph_Buffer *glyph)
{
	uint8_t buf[2];

	if (fread(buf, 1, 2, f) != 2)
		return 0;
	glyph->ascii = buf[0];
	glyph->width = buf[1];
	if (!pick_s(f, &glyph->x))
		return 0;
	if (!pick_s(f, &glyph->y))
		return 0;

	return 1;
}

FTFont *load_font(const char *filename)
{
	FILE *in;
	FTFont *f;
	uint32_t width, height, n_chars;
	size_t i;
	struct Glyph_Buffer buffer;
	struct Glyph *default_glyph;
	unsigned char *tex_data;

	// Open the file and check whether it is any good (a font file
	// starts with "F0")
	in = fopen(filename, "rb");
	if (in == NULL || !(fgetc(in) == 'F' && fgetc(in) == '1')) {
		fprintf(stderr, "FTFont file %s is not valid", filename);
		if (in != NULL)
			fclose(in);
		return NULL;
	}
	f = malloc(sizeof(*f));
	if (f == NULL) {
		fclose(in);
		return NULL;
	}

	f->line_height = 0;
	f->texid = 0;
	f->tex_line_height = 0;

	// Get the texid size, the number of glyphs and the line height.
	i = pick_u(in, &width);
	i += pick_u(in, &height);
	i += pick_u(in, &f->line_height);
	i += pick_u(in, &n_chars);
	if (i != 4) {
		free(f);
		fclose(in);

		return 0;
	}

	f->tex_line_height = (float) f->line_height / height;

	// Make the glyph table.
	f->glyphs = malloc(sizeof(struct Glyph) * n_chars);

	for (i = 0; i != 256; ++i)
		f->table[i] = NULL;

	// Read every glyph, store it in the glyph array and set the right
	// pointer in the table.
	for (i = 0; i < n_chars; ++i) {
		if (pick_glyphbuffer(in, &buffer) == 0) {
			fclose(in);
			free(f->glyphs);
			free(f);
			return NULL;
		}

		f->glyphs[i].tex_x1 = (float) buffer.x / width;
		f->glyphs[i].tex_x2 = (float) (buffer.x + buffer.width) / width;
		f->glyphs[i].tex_y1 = (float) buffer.y / height;
		f->glyphs[i].advance = buffer.width;

		f->table[buffer.ascii] = f->glyphs + i;
	}
	default_glyph = f->table[0xFF];
	// We should have the default character (stored under '\xFF')
	if (default_glyph == NULL) {
		fprintf(stderr, "FTFont (%s) doesn't contain a default glyph(\\xFF)\n", filename);
		default_glyph = f->table[0];	//lets hope
	}

	for (i = 0; i < 256; ++i) {
		if (f->table[i] == NULL)
			f->table[i] = default_glyph;
	}

	// Store the actual texid in an array.
	tex_data = malloc(width * height);
	if (tex_data == NULL) {
		free(f->glyphs);
		free(f);
		fclose(in);
		return NULL;
	}

	if (fread(tex_data, 1, width * height, in) != width * height) {
		fclose(in);
		free(f->glyphs);
		free(f);
		free(tex_data);
		return NULL;
	}
	// Generate an alpha texid with it.
	glGenTextures(1, &f->texid);
	glBindTexture(GL_TEXTURE_2D, f->texid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, tex_data);
	// And delete the original memory block
	free(tex_data);
	fclose(in);

	return f;
}

void ftFree(FTFont * f)
{
	glDeleteTextures(1, &f->texid);
	free(f->glyphs);
	free(f);
}

void ftPrint(const char *str, float x, float y, float size)
{
	FTFont *ft = mainfont;
	float scale = 1.0f;
	float origx = x;

	glBindTexture(GL_TEXTURE_2D, ft->texid);

	glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	scale *= size / mainfont->line_height;
	// Simply draw quads texidd with the current glyph for every
	// character, updating the x position as we go along.
	glBegin(GL_QUADS);
	while (*str) {
		struct Glyph *glyph;
		switch (*str) {
		case '\n':
			y += scale * mainfont->line_height;
			x = origx;
			break;
		case '\t':
			x += scale * 4 * ft->table[' ']->advance;
			break;
		default:
			glyph = ft->table[(unsigned int) *str];

			glTexCoord2f(glyph->tex_x1, glyph->tex_y1);
			glVertex2f(x, y);
			glTexCoord2f(glyph->tex_x2, glyph->tex_y1);
			glVertex2f(x + glyph->advance * scale, y);
			glTexCoord2f(glyph->tex_x2, glyph->tex_y1 + ft->tex_line_height);
			glVertex2f((x + glyph->advance * scale), (y + ft->line_height * scale));
			glTexCoord2f(glyph->tex_x1, glyph->tex_y1 + ft->tex_line_height);
			glVertex2f(x, (y + ft->line_height * scale));

			x += glyph->advance * scale;
			break;
		}
		++str;
	}

	glEnd();
	glPopAttrib();
}

void init_fonts(void)
{
	FTFont *newf;

	if (mainfont != NULL)
		ftFree(mainfont);
	newf = load_font(MAINDIR MAINFONT);
	if (newf == NULL) {
		fprintf(stderr, "Error Can't read font %s\n", MAINFONT);
		exit(3);
	}

	mainfont = newf;
}

//Made by Nils O. Selåsdal
