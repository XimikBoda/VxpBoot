#pragma once
#include "UIBase.h"

typedef void (UIBase::*ActOfClick)();

class UICheckBox : public UIBase
{
public:
	bool value = false;
	const char* label = 0;

	ActOfClick actOfClick = 0;

	UICheckBox(UIBase* parent, int x, int y, int w, int h, const char* label);
	void QDraw(unsigned short* buf) override;
	void OKClick() override;
};

