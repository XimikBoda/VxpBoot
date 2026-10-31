#pragma once
#include "UIBase.h"

typedef int (UIBase::*CheckLimit)(int val);

class UISpinBox : public UIBase {
public:
	int value = 0;

	const char* format = 0;
	const char* label = 0;

	bool active = false;

	CheckLimit cl = 0;
	
	UISpinBox(UIBase* parent, int x, int y, int w, int h, 
		const char* format, const char* label, CheckLimit cl);


	void draw(unsigned short* buf, unsigned char color);

	void Draw(unsigned short* buf) override;
	void QDraw(unsigned short* buf) override;
	void KeyboardEvent(int event, int keycode) override;
	void OKClick() override;
};

