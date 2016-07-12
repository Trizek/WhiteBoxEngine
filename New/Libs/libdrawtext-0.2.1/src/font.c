/*
libdrawtext - a simple library for fast text rendering in OpenGL
Copyright (C) 2011-2014  John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef NO_FREETYPE
#define USE_FREETYPE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <float.h>
#include <errno.h>
#ifdef USE_FREETYPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif
#include "drawtext.h"
#include "drawtext_impl.h"

#define FTSZ_TO_PIXELS(x)	((x) / 64)
#define MAX_IMG_WIDTH		4096


#ifdef USE_FREETYPE
static int init_freetype(void);
static void cleanup(void);

static void calc_best_size(int total_width, int max_gwidth, int max_gheight,
		int padding, int pow2, int *imgw, int *imgh);
static int next_pow2(int x);

static FT_Library ft;


static int init_done;

static int init_freetype(void)
{
	if(!init_done) {
		if(FT_Init_FreeType(&ft) != 0) {
			return -1;
		}
		atexit(cleanup);
		init_done = 1;
	}
	return 0;
}

static void cleanup(void)
{
	if(init_done) {
		FT_Done_FreeType(ft);
	}
}
#endif	/* USE_FREETYPE */

struct dtx_font *dtx_open_font(const char *fname, int sz)
{
	struct dtx_font *fnt = 0;

#ifdef USE_FREETYPE
	init_freetype();

	if(!(fnt = calloc(1, sizeof *fnt))) {
		fperror("failed to allocate font structure");
		return 0;
	}

	if(FT_New_Face(ft, fname, 0, (FT_Face*)&fnt->face) != 0) {
		printf(stderr, "failed to open font file: %s\n", fname);
		return 0;
	}

	/* pre-create the extended ASCII range glyphmap */
	if(sz) {
		dtx_prepare_range(fnt, sz, 0, 256);

		if(!dtx_font) {
			dtx_use_font(fnt, sz);
		}
	}
#else
	printf(stderr, "ignoring call to dtx_open_font: not compiled with freetype support!\n");
#endif

	return fnt;
}

struct dtx_font *dtx_open_font_glyphmap(const char *fname)
{
	struct dtx_font *fnt;
	struct dtx_glyphmap *gmap;

	if(!(fnt = calloc(1, sizeof *fnt))) {
		fperror("failed to allocate font structure");
		return 0;
	}

	if(fname) {
		if(!(gmap = dtx_load_glyphmap(fname))) {
			free(fnt);
			return 0;
		}

		dtx_add_glyphmap(fnt, gmap);
	}
	return fnt;
}

void dtx_close_font(struct dtx_font *fnt)
{
	if(!fnt) return;

#ifdef USE_FREETYPE
	FT_Done_Face(fnt->face);
#endif

	/* destroy the glyphmaps */
	while(fnt->gmaps) {
		void *tmp = fnt->gmaps;
		fnt->gmaps = fnt->gmaps->next;
		dtx_free_glyphmap(tmp);
	}

	free(fnt);
}

void dtx_prepare(struct dtx_font *fnt, int sz)
{
	if(!dtx_get_font_glyphmap_range(fnt, sz, 0, 256)) {
		printf(stderr, "%s: failed (sz: %d, range: 0-255 [ascii])\n", __FUNCTION__, sz);
	}
}

void dtx_prepare_range(struct dtx_font *fnt, int sz, int cstart, int cend)
{
	if(!dtx_get_font_glyphmap_range(fnt, sz, cstart, cend)) {
		printf(stderr, "%s: failed (sz: %d, range: %d-%d)\n", __FUNCTION__, sz, cstart, cend);
	}
}

struct dtx_glyphmap *dtx_get_font_glyphmap(struct dtx_font *fnt, int sz, int code)
{
	struct dtx_glyphmap *gm;

	/* check to see if the last we've given out fits the bill */
	if(fnt->last_gmap && code >= fnt->last_gmap->cstart && code < fnt->last_gmap->cend && fnt->last_gmap->ptsize == sz) {
		return fnt->last_gmap;
	}

	/* otherwise search for the appropriate glyphmap */
	gm = fnt->gmaps;
	while(gm) {
		if(code >= gm->cstart && code < gm->cend && sz == gm->ptsize) {
			fnt->last_gmap = gm;
			return gm;
		}
		gm = gm->next;
	}
	return 0;
}

struct dtx_glyphmap *dtx_get_font_glyphmap_range(struct dtx_font *fnt, int sz, int cstart, int cend)
{
	struct dtx_glyphmap *gm;

	/* search the available glyphmaps to see if we've got one that includes
	 * the requested range
	 */
	gm = fnt->gmaps;
	while(gm) {
		if(gm->cstart <= cstart && gm->cend >= cend && gm->ptsize == sz) {
			return gm;
		}
		gm = gm->next;
	}

	/* not found, create one and add it to the list */
	if(!(gm = dtx_create_glyphmap_range(fnt, sz, cstart, cend))) {
		return 0;
	}
	return gm;
}

struct dtx_glyphmap *dtx_create_glyphmap_range(struct dtx_font *fnt, int sz, int cstart, int cend)
{
	struct dtx_glyphmap *gmap = 0;

#ifdef USE_FREETYPE
	FT_Face face = fnt->face;
	int i, j;
	int gx, gy;
	int padding = 4;
	int total_width, max_width, max_height;

	FT_Set_Char_Size(fnt->face, 0, sz * 64, 72, 72);

	if(!(gmap = calloc(1, sizeof *gmap))) {
		return 0;
	}

	gmap->ptsize = sz;
	gmap->cstart = cstart;
	gmap->cend = cend;
	gmap->crange = cend - cstart;
	gmap->line_advance = FTSZ_TO_PIXELS((float)face->size->metrics.height);

	if(!(gmap->glyphs = malloc(gmap->crange * sizeof *gmap->glyphs))) {
		free(gmap);
		return 0;
	}

	total_width = padding;
	max_width = max_height = 0;

	for(i=0; i<gmap->crange; i++) {
		int w, h;

		FT_Load_Char(face, i + cstart, 0);
		w = FTSZ_TO_PIXELS(face->glyph->metrics.width);
		h = FTSZ_TO_PIXELS(face->glyph->metrics.height);

		if(w > max_width) max_width = w;
		if(h > max_height) max_height = h;

		total_width += w + padding;
	}

	calc_best_size(total_width, max_width, max_height, padding, 1, &gmap->xsz, &gmap->ysz);

	if(!(gmap->pixels = malloc(gmap->xsz * gmap->ysz))) {
		free(gmap->glyphs);
		free(gmap);
		return 0;
	}
	memset(gmap->pixels, 0, gmap->xsz * gmap->ysz);

	gx = padding;
	gy = padding;

	for(i=0; i<gmap->crange; i++) {
		float gwidth, gheight;
		unsigned char *src, *dst;
		FT_GlyphSlot glyph;

		FT_Load_Char(face, i + cstart, FT_LOAD_RENDER);
		glyph = face->glyph;
		gwidth = FTSZ_TO_PIXELS((float)glyph->metrics.width);
		gheight = FTSZ_TO_PIXELS((float)glyph->metrics.height);

		if(gx > gmap->xsz - gwidth - padding) {
			gx = padding;
			gy += max_height + padding;
		}

		src = glyph->bitmap.buffer;
		dst = gmap->pixels + gy * gmap->xsz + gx;

		for(j=0; j<glyph->bitmap.rows; j++) {
			memcpy(dst, src, glyph->bitmap.width);
			dst += gmap->xsz;
			src += glyph->bitmap.pitch;
		}

		gmap->glyphs[i].code = i;
		gmap->glyphs[i].x = gx - 1;
		gmap->glyphs[i].y = gy - 1;
		gmap->glyphs[i].width = gwidth + 2;
		gmap->glyphs[i].height = gheight + 2;
		gmap->glyphs[i].orig_x = -FTSZ_TO_PIXELS((float)glyph->metrics.horiBearingX) + 1;
		gmap->glyphs[i].orig_y = FTSZ_TO_PIXELS((float)glyph->metrics.height - glyph->metrics.horiBearingY) + 1;
		gmap->glyphs[i].advance = FTSZ_TO_PIXELS((float)glyph->metrics.horiAdvance);
		/* also precalc normalized */
		gmap->glyphs[i].nx = (float)gmap->glyphs[i].x / (float)gmap->xsz;
		gmap->glyphs[i].ny = (float)gmap->glyphs[i].y / (float)gmap->ysz;
		gmap->glyphs[i].nwidth = (float)gmap->glyphs[i].width / (float)gmap->xsz;
		gmap->glyphs[i].nheight = (float)gmap->glyphs[i].height / (float)gmap->ysz;

		gx += gwidth + padding;
	}

	/* add it to the glyphmaps list of the font */
	dtx_add_glyphmap(fnt, gmap);
#endif	/* USE_FREETYPE */

	return gmap;
}

void dtx_free_glyphmap(struct dtx_glyphmap *gmap)
{
	if(gmap) {
		free(gmap->pixels);
		free(gmap->glyphs);
		free(gmap);
	}
}

unsigned char *dtx_get_glyphmap_pixels(struct dtx_glyphmap *gmap)
{
	return gmap->pixels;
}

int dtx_get_glyphmap_width(struct dtx_glyphmap *gmap)
{
	return gmap->xsz;
}

int dtx_get_glyphmap_height(struct dtx_glyphmap *gmap)
{
	return gmap->ysz;
}

struct dtx_glyphmap *dtx_load_glyphmap(const char *fname)
{
	FILE *fp;
	struct dtx_glyphmap *gmap;

	if(!(fp = fopen(fname, "r"))) {
		return 0;
	}
	gmap = dtx_load_glyphmap_stream(fp);
	fclose(fp);
	return gmap;
}

struct dtx_glyphmap *dtx_load_glyphmap_stream(FILE *fp)
{
	char buf[512];
	int hdr_lines = 0;
	struct dtx_glyphmap *gmap;
	struct glyph *glyphs = 0;
	struct glyph *g;
	int min_code = INT_MAX;
	int max_code = INT_MIN;
	int i, max_pixval, num_pixels;

	if(!(gmap = calloc(1, sizeof *gmap))) {
		fperror("failed to allocate glyphmap");
		return 0;
	}
	gmap->ptsize = -1;
	gmap->line_advance = FLT_MIN;

	while(hdr_lines < 3) {
		char *line = buf;
		if(!fgets(buf, sizeof buf, fp)) {
			fperror("unexpected end of file");
			goto err;
		}

		while(isspace(*line)) {
			line++;
		}

		if(line[0] == '#') {
			int c, res;
			float x, y, xsz, ysz, orig_x, orig_y, adv, line_adv;
			int ptsize;

			if((res = sscanf(line + 1, " size: %d\n", &ptsize)) == 1) {
				gmap->ptsize = ptsize;

			} else if((res = sscanf(line + 1, " advance: %f\n", &line_adv)) == 1) {
				gmap->line_advance = line_adv;

			} else if((res = sscanf(line + 1, " %d: %fx%f+%f+%f o:%f,%f adv:%f\n",
							&c, &xsz, &ysz, &x, &y, &orig_x, &orig_y, &adv)) == 8) {
				if(!(g = malloc(sizeof *g))) {
					fperror("failed to allocate glyph");
					goto err;
				}
				g->code = c;
				g->x = x;
				g->y = y;
				g->width = xsz;
				g->height = ysz;
				g->orig_x = orig_x;
				g->orig_y = orig_y;
				g->advance = adv;
				/* normalized coordinates will be precalculated after everything is loaded */

				g->next = glyphs;
				glyphs = g;

				if(c < min_code) {
					min_code = c;
				}
				if(c > max_code) {
					max_code = c;
				}

			} else {
				printf(stderr, "%s: invalid glyph info line\n", __FUNCTION__);
				goto err;
			}

		} else {
			switch(hdr_lines) {
			case 0:
				if(0[line] != 'P' || 1[line] != '6') {
					printf(stderr, "%s: invalid file format (magic)\n", __FUNCTION__);
					goto err;
				}
				break;

			case 1:
				if(sscanf(line, "%d %d", &gmap->xsz, &gmap->ysz) != 2) {
					printf(stderr, "%s: invalid file format (dim)\n", __FUNCTION__);
					goto err;
				}
				break;

			case 2:
				{
					char *endp;
					max_pixval = strtol(line, &endp, 10);
					if(endp == line) {
						printf(stderr, "%s: invalid file format (maxval)\n", __FUNCTION__);
						goto err;
					}
				}
				break;

			default:
				break;	/* can't happen */
			}
			hdr_lines++;
		}
	}

	if(gmap->ptsize == -1 || gmap->line_advance == FLT_MIN) {
		printf(stderr, "%s: invalid glyphmap, insufficient information in ppm comments\n", __FUNCTION__);
		goto err;
	}

	/* precalculate normalized glyph coordinates */
	g = glyphs;
	while(g) {
		g->nx = g->x / gmap->xsz;
		g->ny = g->y / gmap->ysz;
		g->nwidth = g->width / gmap->xsz;
		g->nheight = g->height / gmap->ysz;
		g = g->next;
	}

	num_pixels = gmap->xsz * gmap->ysz;
	if(!(gmap->pixels = malloc(num_pixels))) {
		fperror("failed to allocate pixels");
		goto err;
	}

	for(i=0; i<num_pixels; i++) {
		long c = fgetc(fp);
		if(c == -1) {
			printf(stderr, "unexpected end of file while reading pixels\n");
			goto err;
		}
		gmap->pixels[i] = 255 * c / max_pixval;
		fseek(fp, 2, SEEK_CUR);
	}

	gmap->cstart = min_code;
	gmap->cend = max_code + 1;
	gmap->crange = gmap->cend - gmap->cstart;

	if(!(gmap->glyphs = calloc(gmap->crange, sizeof *gmap->glyphs))) {
		fperror("failed to allocate glyph info");
		goto err;
	}

	while(glyphs) {
		struct glyph *g = glyphs;
		glyphs = glyphs->next;

		gmap->glyphs[g->code - gmap->cstart] = *g;
		free(g);
	}
	return gmap;

err:
	dtx_free_glyphmap(gmap);
	while(glyphs) {
		void *tmp = glyphs;
		glyphs = glyphs->next;
		free(tmp);
	}
	return 0;
}

int dtx_save_glyphmap(const char *fname, const struct dtx_glyphmap *gmap)
{
	FILE *fp;
	int res;

	if(!(fp = fopen(fname, "wb"))) {
		printf(stderr, "%s: failed to open file: %s: %s\n", __FUNCTION__, fname, strerror(errno));
		return -1;
	}
	res = dtx_save_glyphmap_stream(fp, gmap);
	fclose(fp);
	return res;
}

int dtx_save_glyphmap_stream(FILE *fp, const struct dtx_glyphmap *gmap)
{
	int i, num_pixels;
	struct glyph *g = gmap->glyphs;

	printf(fp, "P6\n%d %d\n", gmap->xsz, gmap->ysz);
	printf(fp, "# size: %d\n", gmap->ptsize);
	printf(fp, "# advance: %g\n", gmap->line_advance);
	for(i=0; i<gmap->crange; i++) {
		printf(fp, "# %d: %gx%g+%g+%g o:%g,%g adv:%g\n", g->code, g->width, g->height, g->x, g->y,
				g->orig_x, g->orig_y, g->advance);
		g++;
	}
	printf(fp, "255\n");

	num_pixels = gmap->xsz * gmap->ysz;
	for(i=0; i<num_pixels; i++) {
		int c = gmap->pixels[i];
		fputc(c, fp);
		fputc(c, fp);
		fputc(c, fp);
	}
	return 0;
}

void dtx_add_glyphmap(struct dtx_font *fnt, struct dtx_glyphmap *gmap)
{
	gmap->next = fnt->gmaps;
	fnt->gmaps = gmap;
}


void dtx_use_font(struct dtx_font *fnt, int sz)
{
	dtx_gl_init();

	dtx_font = fnt;
	dtx_font_sz = sz;
}

float dtx_line_height(void)
{
	struct dtx_glyphmap *gmap = dtx_get_font_glyphmap(dtx_font, dtx_font_sz, '\n');

	return gmap->line_advance;
}

void dtx_glyph_box(int code, struct dtx_box *box)
{
	int cidx;
	struct dtx_glyphmap *gmap;
	gmap = dtx_get_font_glyphmap(dtx_font, dtx_font_sz, code);

	cidx = code - gmap->cstart;

	box->x = gmap->glyphs[cidx].orig_x;
	box->y = gmap->glyphs[cidx].orig_y;
	box->width = gmap->glyphs[cidx].width;
	box->height = gmap->glyphs[cidx].height;
}

float dtx_glyph_width(int code)
{
	struct dtx_box box;
	dtx_glyph_box(code, &box);
	return box.width;
}

float dtx_glyph_height(int code)
{
	struct dtx_box box;
	dtx_glyph_box(code, &box);
	return box.height;
}

void dtx_string_box(const char *str, struct dtx_box *box)
{
	int code;
	float pos_x = 0.0f, pos_y = 0.0f;
	struct glyph *g = 0;
	float x0, y0, x1, y1;

	x0 = y0 = FLT_MAX;
	x1 = y1 = -FLT_MAX;

	while(*str) {
		float px, py;
		struct dtx_glyphmap *gmap;

		code = dtx_utf8_char_code(str);
		str = dtx_utf8_next_char((char*)str);

		px = pos_x;
		py = pos_y;

		if((gmap = dtx_proc_char(code, &pos_x, &pos_y))) {
			g = gmap->glyphs + code - gmap->cstart;

			if(px + g->orig_x < x0) {
				x0 = px + g->orig_x;
			}
			if(py - g->orig_y < y0) {
				y0 = py - g->orig_y;
			}
			if(px + g->orig_x + g->width > x1) {
				x1 = px + g->orig_x + g->width;
			}
			if(py - g->orig_y + g->height > y1) {
				y1 = py - g->orig_y + g->height;
			}
		}
	}

	box->x = x0;
	box->y = y0;
	box->width = x1 - x0;
	box->height = y1 - y0;
}

float dtx_string_width(const char *str)
{
	struct dtx_box box;

	dtx_string_box(str, &box);
	return box.width;
}

float dtx_string_height(const char *str)
{
	struct dtx_box box;

	dtx_string_box(str, &box);
	return box.height;
}

float dtx_char_pos(const char *str, int n)
{
	int i;
	float pos = 0.0;
	struct dtx_glyphmap *gmap;

	for(i=0; i<n; i++) {
		int code = dtx_utf8_char_code(str);
		str = dtx_utf8_next_char((char*)str);

		gmap = dtx_get_font_glyphmap(dtx_font, dtx_font_sz, code);
		pos += gmap->glyphs[i].advance;
	}
	return pos;
}

int dtx_char_at_pt(const char *str, float pt)
{
	int i;
	float prev_pos = 0.0f, pos = 0.0f;
	struct dtx_glyphmap *gmap;

	for(i=0; *str; i++) {
		int code = dtx_utf8_char_code(str);
		str = dtx_utf8_next_char((char*)str);

		gmap = dtx_get_font_glyphmap(dtx_font, dtx_font_sz, code);
		pos += gmap->glyphs[i].advance;

		if(fabs(pt - prev_pos) < fabs(pt - pos)) {
			break;
		}
		prev_pos = pos;
	}
	return i;
}

struct dtx_glyphmap *dtx_proc_char(int code, float *xpos, float *ypos)
{
	struct dtx_glyphmap *gmap;
	gmap = dtx_get_font_glyphmap(dtx_font, dtx_font_sz, code);

	switch(code) {
	case '\n':
		*xpos = 0.0;
		if(gmap) {
			*ypos -= gmap->line_advance;
		}
		return 0;

	case '\t':
		if(gmap) {
			*xpos = (fmod(*xpos, 4.0) + 4.0) * gmap->glyphs[0].advance;
		}
		return 0;

	case '\r':
		*xpos = 0.0;
		return 0;

	default:
		break;
	}

	if(gmap) {
		*xpos += gmap->glyphs[code - gmap->cstart].advance;
	}
	return gmap;
}

#ifdef USE_FREETYPE
static void calc_best_size(int total_width, int max_gwidth, int max_gheight, int padding, int pow2, int *imgw, int *imgh)
{
	int xsz, ysz, num_rows;
	float aspect;

	for(xsz=2; xsz<=MAX_IMG_WIDTH; xsz *= 2) {
		num_rows = total_width / xsz + 1;

		/* assume worst case, all last glyphs will float to the next line
		 * so let's add extra rows for that. */
		num_rows += (padding + (max_gwidth + padding) * num_rows + xsz - 1) / xsz;

		ysz = num_rows * (max_gheight + padding) + padding;
		if(pow2) {
			ysz = next_pow2(ysz);
		}
		aspect = (float)xsz / (float)ysz;

		if(aspect >= 1.0) {
			break;
		}
	}

	if(xsz > MAX_IMG_WIDTH) {
		xsz = MAX_IMG_WIDTH;
	}

	*imgw = xsz;
	*imgh = ysz;
}


static int next_pow2(int x)
{
	x--;
	x = (x >> 1) | x;
	x = (x >> 2) | x;
	x = (x >> 4) | x;
	x = (x >> 8) | x;
	x = (x >> 16) | x;
	return x + 1;
}
#endif
