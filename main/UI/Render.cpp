#include "Render.h"
#include "vmsys.h"
#include "vmio.h"
#include "vmgraph.h"
#include "string.h"
extern VMUINT16* layer_bufs[2];

const int pro_char_width = 6, pro_char_height = 11;



namespace Render {
	void draw_text_white_centered(unsigned short* buf, int x, int y, int w, const char* str) {
		int len = strlen(str);
		draw_text_white_by_len(buf, x + (w-pro_char_width * len) / 2, y, str, len);
	}

	void draw_text_white(unsigned short* buf, int x, int y, const char* str) {
		for (int ci = 0; *str; ++ci, ++str) {
			const unsigned char* font_ch = ProFont6x11 + 5 + 12 * (*str) + 1;

			int t_char_width = pro_char_width;
			if (x + ci * pro_char_width + pro_char_width > screen_w) {
				t_char_width = screen_w - x + ci * pro_char_width;
				if (t_char_width <= 0)
					break;
			}
			for (int i = 0; i < pro_char_height; ++i) {
				unsigned short* scr_buf = (unsigned short*)buf + x + ci * pro_char_width + (y + i) * screen_w;
				if (y + i > 0 && y + i < screen_h)
					for (int j = 0; j < t_char_width; ++j)
						if ((((*font_ch) >> j) & 1) && (x + ci * pro_char_width + j) >= 0 && (x + ci * pro_char_width + j) < screen_w)
							scr_buf[j] = 0xFFFF;
				++font_ch;
			}
		}
	}
	void draw_text_white_by_len(unsigned short* buf, int x, int y, const char* str, int len) {
		for (int ci = 0; ci < len; ++ci) {
			const unsigned char* font_ch = ProFont6x11 + 5 + 12 * str[ci] + 1;

			int t_char_width = pro_char_width;
			if (x + ci * pro_char_width + pro_char_width > screen_w) {
				t_char_width = screen_w - x + ci * pro_char_width;
				if (t_char_width <= 0)
					break;
			}
			for (int i = 0; i < pro_char_height; ++i) {
				unsigned short* scr_buf = (unsigned short*)buf + x + ci * pro_char_width + (y + i) * screen_w;
				if (y + i > 0 && y + i < screen_h)
					for (int j = 0; j < t_char_width; ++j)
						if ((((*font_ch) >> j) & 1) && (x + ci * pro_char_width + j) >= 0 && (x + ci * pro_char_width + j) < screen_w)
							scr_buf[j] = 0xFFFF;
				++font_ch;
			}
		}
	}
	void draw_text_white_with_black_by_len(unsigned short* buf, int x, int y, const char* str, int len) {
		for (int ci = 0; ci < len && str[ci]; ++ci) {//todo
			const unsigned char* font_ch = ProFont6x11 + 5 + 12 * str[ci] + 1;

			int t_char_width = pro_char_width;
			if (x + ci * pro_char_width + pro_char_width > screen_w) {
				t_char_width = screen_w - x + ci * pro_char_width;
				if (t_char_width <= 0)
					break;
			}
			for (int i = 0; i < pro_char_height; ++i) {
				unsigned short* scr_buf = (unsigned short*)buf + x + ci * pro_char_width + (y + i) * screen_w;
				if (y + i > 0 && y + i < screen_h)
					for (int j = 0; j < t_char_width; ++j)
						if ((x + ci * pro_char_width + j) >= 0 && (x + ci * pro_char_width + j) < screen_w)
							scr_buf[j] = ((((*font_ch) >> j) & 1) ? 0xFFFF : 0x0000);
				++font_ch;
			}
		}
	}

	void draw_fill_rect(unsigned short* buf, int x, int y, int w, int h, unsigned short color) {
		vm_graphic_fill_rect((VMUINT8*)buf, x, y, w, h, color, color);
	}

	void draw_fill_rect_with_borders(unsigned short* buf, int x, int y, int w, int h, unsigned short fill_color, unsigned short border_color) {
		vm_graphic_fill_rect((VMUINT8*)buf, x, y, w, h, border_color, fill_color);
	}

	void draw_triangle(unsigned short* buf, int x1, int y1, int x2, int x3, int y3, unsigned short color) {
		// 1---2
		//  \ /
		//   3
		int miy = y1 < y3 ? y1 : y3;
		int may = y1 > y3 ? y1 : y3;
		for (int y = miy; y <= may; ++y) {
			if (y < 0 || y >= screen_h)
				continue;
			int mix = (y - y1) * (x3 - x1) / (y3 - y1) + x1;
			int max_ = (y - y1) * (x3 - x2) / (y3 - y1) + x2;
			for (int x = mix; x <= max_; ++x) {
				if (x < 0 || x >= screen_w)
					continue;
				buf[x + y * screen_w] = color;
			}
		}
	}
}