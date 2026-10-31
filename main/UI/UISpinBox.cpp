#include "UISpinBox.h"
#include "string.h"
#include "UIEngine.h"
#include "T2Input.h"
#include "Render.h"
#include <vmio.h>
#include <vmgraph.h>
#include <vmstdlib.h>

extern UIEngine uiengine;

UISpinBox::UISpinBox(UIBase* parent, int x, int y, int w, int h, 
	const char* format, const char* label, CheckLimit cl)
{
	this->parent = parent;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->format = format;
	this->label = label;
	this->cl = cl;
}

void UISpinBox::draw(unsigned short* buf, unsigned char color) {
	char tmp[32] = "";
	sprintf(tmp, format, value);

	int ty = y + (h - 11) / 2 + 1;

	Render::draw_text_white(buf, x + 3, ty, tmp);

	Render::draw_fill_rect_with_borders(buf, x + w - h + 1, y, h - 1, h, color, 0xFFFF);

	{
		int t_w = h / 2 - 1, t_h = (h - 4 - 3) / 2;
		int t_x1 = x + w - h + h / 4 + 1;
		int t_x2 = t_x1 + t_w;
		int t_x3 = t_x1 + t_w / 2;
		int t_y_m = y + h / 2;

		Render::draw_triangle(buf, t_x1, t_y_m - 1, t_x2, t_x3, t_y_m - 1 - t_h, 0xFFFF);
		Render::draw_triangle(buf, t_x1, t_y_m + 1, t_x2, t_x3, t_y_m + 1 + t_h, 0xFFFF);
	}

	{
		int text_x = x + w + 5, text_y = y + h - 11;
		Render::draw_fill_rect(buf, text_x, text_y, 240, 11, 0x0000);
		Render::draw_text_white(buf, text_x, text_y, label);
	}
}

void UISpinBox::Draw(unsigned short* buf)
{
	Render::draw_fill_rect_with_borders(buf, x, y, w, h, 0x0000, 0xFFFF);

	draw(buf, gray_color);
}

void UISpinBox::QDraw(unsigned short* buf) 
{
	unsigned char color = isFocused ? gray_color : 0x0000;
	Render::draw_fill_rect_with_borders(buf, x, y, w, h, color, 0xFFFF);

	draw(buf, color);
}

void UISpinBox::KeyboardEvent(int event, int keycode)
{
	switch (event)
	{
	case VM_KEY_EVENT_UP:
		switch (keycode)
		{
		case VM_KEY_UP:
			++value;
			if (parent && cl)
				value = (parent->*cl)(value);
			break;
		case VM_KEY_DOWN:
			--value;
			if (parent && cl)
				value = (parent->*cl)(value);
			break;
		case VM_KEY_OK:
		case VM_KEY_RIGHT_SOFTKEY:
			active = false;
			uiengine.PopUI();
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void UISpinBox::OKClick()
{
	active = true;
	uiengine.PushUI((UIBase*)this);
}
