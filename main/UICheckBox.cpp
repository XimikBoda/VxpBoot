#include "UICheckBox.h"
#include "Render.h"
#include "string.h"
#include <vmio.h>

extern VMINT layer_hdl[2];

UICheckBox::UICheckBox(UIBase* parent, int x, int y, int w, int h, const char* label)
{
	this->parent = parent;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = h;
	this->label = label;
}

void UICheckBox::QDraw(unsigned short* buf)
{
	unsigned char color = isFocused ? gray_color : 0x0000;
	Render::draw_fill_rect_with_borders(buf, x, y, w, h, color, 0xFFFF);

	if (value) {
		vm_graphic_point points[5] =
		{
			{2, 5},
			{6, 8},
			{14, -5},
			{14, -3},
			{6, 12}
		};

		for (int i = 0; i < 5; ++i)
			points[i].x += x, points[i].y += y;

		vm_graphic_color vm_color = { VM_COLOR_GREEN };
		vm_graphic_setcolor(&vm_color);
		vm_graphic_fill_polygon(layer_hdl[0], points, 5);
	}

	Render::draw_text_white(buf, x + w + 5, y + h - 11, label);
}

void UICheckBox::OKClick()
{
	value = !value;
}
