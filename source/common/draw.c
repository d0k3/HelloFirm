// Copyright 2013 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "draw.h"

#define FONT_MAX_WIDTH 8
#define FONT_MAX_HEIGHT 10

static u32 font_width = 0;
static u32 font_height = 0;
static u32 line_height = 0;
static u8 font_bin[FONT_MAX_HEIGHT * 256];


u8* GetFontFromPbm(const void* pbm, const u32 pbm_size, u32* w, u32* h) {
	char* hdr = (char*) pbm;
	u32 hdr_max_size = min(512, pbm_size);
	u32 pbm_w = 0;
	u32 pbm_h = 0;
	
	// minimum size
	if (hdr_max_size < 7) return NULL;
	
	// check header magic, then skip over
	if (strncmp(hdr, "P4\n", 3) != 0) return NULL;
	
	// skip any comments
	u32 p = 3;
	while (hdr[p] == '#') {
		while (hdr[p++] != '\n') {
			if (p >= hdr_max_size) return NULL;
		}
	}
	
	// parse width
	while ((hdr[p] >= '0') && (hdr[p] <= '9')) {
		if (p >= hdr_max_size) return NULL;
		pbm_w *= 10;
		pbm_w += hdr[p++] - '0';
	}
	
	// whitespace
	if ((hdr[p++] != ' ') || (p >= hdr_max_size))
		return NULL;
	
	// parse height
	while ((hdr[p] >= '0') && (hdr[p] <= '9')) {
		if (p >= hdr_max_size) return NULL;
		pbm_h *= 10;
		pbm_h += hdr[p++] - '0';
	}
	
	// line break
	if ((hdr[p++] != '\n') || (p >= hdr_max_size))
		return NULL;
	
	// check sizes
	if (pbm_w <= 8) { // 1x256 format
		if ((pbm_w > FONT_MAX_WIDTH) || (pbm_h % 256) ||
			((pbm_h / 256) > FONT_MAX_HEIGHT) ||
			(pbm_h != (pbm_size - p)))
			return NULL;
	} else { // 16x16 format
		if ((pbm_w % 16) || (pbm_h % 16) ||
			((pbm_w / 16) > FONT_MAX_WIDTH) ||
			((pbm_h / 16) > FONT_MAX_HEIGHT) ||
			((pbm_h * pbm_w / 8) != (pbm_size - p)))
			return NULL;
	}
	
	// all good
	if (w) *w = pbm_w;
	if (h) *h = pbm_h;
	return (u8*) pbm + p;
}

// sets the font from a given PBM
bool SetFontFromPbm(const void* pbm, u32 pbm_size) {
	u32 w, h;
	u8* ptr = NULL;
	
	if (pbm)
		ptr = GetFontFromPbm(pbm, pbm_size, &w, &h);
	
	if (!ptr) {
		return false;
	} else if (w > 8) {
		font_width = w / 16;
		font_height = h / 16;
		memset(font_bin, 0x00, w * h / 8);
		
		for (u32 cy = 0; cy < 16; cy++) {
			for (u32 row = 0; row < font_height; row++) {
				for (u32 cx = 0; cx < 16; cx++) {
					u32 bp0 = (cx * font_width) >> 3;
					u32 bm0 = (cx * font_width) % 8;
					u8 byte = ((ptr[bp0] << bm0) | (ptr[bp0+1] >> (8 - bm0))) & (0xFF << (8 - font_width));
					font_bin[(((cy << 4) + cx) * font_height) + row] = byte;
				}
				ptr += font_width << 1;
			}
		}
	} else {
		font_width = w;
		font_height = h / 256;
		memcpy(font_bin, ptr, h);
	}
	
	line_height = min(10, font_height + 2);
	return true;
}

void ClearScreen(u8* screen, int width, int color)
{
	if (color == COLOR_TRANSPARENT) color = COLOR_BLACK;
	for (int i = 0; i < (width * SCREEN_HEIGHT); i++) {
		*(screen++) = color >> 16;  // B
		*(screen++) = color >> 8;   // G
		*(screen++) = color & 0xFF; // R
	}
}

void ClearScreenFull(bool clear_top, bool clear_bottom)
{
	if (clear_top)
		ClearScreen(TOP_SCREEN, SCREEN_WIDTH_TOP, STD_COLOR_BG);
	if (clear_bottom)
		ClearScreen(BOT_SCREEN, SCREEN_WIDTH_BOT, STD_COLOR_BG);
}

void DrawCharacter(u8* screen, int character, int x, int y, int color, int bgcolor)
{
	for (int yy = 0; yy < (int) font_height; yy++) {
		int xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_HEIGHT);
		int yDisplacement = ((SCREEN_HEIGHT - (y + yy) - 1) * BYTES_PER_PIXEL);
		u8* screenPos = screen + xDisplacement + yDisplacement;

		u8 charPos = font_bin[character * font_height + yy];
		for (int xx = 7; xx >= (8 - (int) font_width); xx--) {
			if ((charPos >> xx) & 1) {
				*(screenPos + 0) = color >> 16;  // B
				*(screenPos + 1) = color >> 8;   // G
				*(screenPos + 2) = color & 0xFF; // R
			} else if (bgcolor != COLOR_TRANSPARENT) {
				*(screenPos + 0) = bgcolor >> 16;  // B
				*(screenPos + 1) = bgcolor >> 8;   // G
				*(screenPos + 2) = bgcolor & 0xFF; // R
			}
			screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT;
		}
	}
}

void DrawString(u8* screen, const char *str, int x, int y, int color, int bgcolor)
{
	for (size_t i = 0; i < strlen(str); i++)
		DrawCharacter(screen, str[i], x + i * font_width, y, color, bgcolor);
}

void DrawStringF(int x, int y, bool use_top, const char *format, ...)
{
	char str[512] = { 0 }; // 512 should be more than enough
	va_list va;

	va_start(va, format);
	vsnprintf(str, 512, format, va);
	va_end(va);

	DrawString((use_top) ? TOP_SCREEN : BOT_SCREEN, str, x, y, STD_COLOR_FONT, STD_COLOR_BG);
}

void DrawRectangle(u8* screen, int x, int y, int width, int height, int color)
{
	for (int yy = 0; yy < height; yy++) {
		int xDisplacement = (x * BYTES_PER_PIXEL * SCREEN_HEIGHT);
		int yDisplacement = ((SCREEN_HEIGHT - (y + yy) - 1) * BYTES_PER_PIXEL);
		u8* screenPos = screen + xDisplacement + yDisplacement;
		for (int xx = width - 1; xx >= 0; xx--) {
			*(screenPos + 0) = color >> 16;  // B
			*(screenPos + 1) = color >> 8;   // G
			*(screenPos + 2) = color & 0xFF; // R
			screenPos += BYTES_PER_PIXEL * SCREEN_HEIGHT;
		}
	}
}