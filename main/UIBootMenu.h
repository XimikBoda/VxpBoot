#pragma once
#include "UI/UIBase.h"
#include "UI/UIMenuBase.h"
#include "UI/UIButton.h"
#include "UI/UITextBox.h"
#include "UI/UIComboBox.h"
#include "UI/UICheckBox.h"
#include "UI/UISpinBox.h"
#include <string>
#include "vmstdlib.h"

struct console_mode_t {
	const char* name;
	const char* cmd;
};

const console_mode_t console_modes[12] = 
{
	{"Screen",						"console=tty0"},
	{"USB",							"console=ttyGS0"},
	{"UART",						"console=ttyS0,115200n8"},

	{"Screen (main) + USB",			"console=ttyGS0 console=tty0"},
	{"Screen (main) + UART",		"console=ttyS0,115200n8 console=tty0"},

	{"USB (main) + Screen",			"console=tty0 console=ttyGS0"},
	{"USB (main) + UART",			"console=ttyS0,115200n8 console=ttyGS0"},

	{"UART (main) + Screen",		"console=tty0 console=ttyS0,115200n8"},
	{"UART (main) + USB",			"console=ttyGS0 console=ttyS0,115200n8"},

	{"Screen (main) + USB + UART",	"console=ttyGS0 console=ttyS0,115200n8 console=tty0"},
	{"USB (main) + Screen + UART",	"console=ttyS0,115200n8 console=tty0 console=ttyGS0"},
	{"UART (main) + Screen + USB",	"console=tty0 console=ttyGS0 console=ttyS0,115200n8"},
};

class UIBootMenu : public UIMenuBase
{
	char detected_cpu[100] = "";
	int detected_cpu_num = 0;

	UIComboBox cpuSelector = UIComboBox(this, 5, 0, 230, 14, 0,
		(GetListLen)&UIBootMenu::CpusGetLen,
		(GetListElement)&UIBootMenu::CpusGetElement);

	int detected_ram = 0;
	int detected_ram_mb = 0;

	UISpinBox ramSpin = UISpinBox(this, 5, 0, 50, 14, "%d MB", "", (CheckLimit)&UIBootMenu::CheckRamLimit);

	UIComboBox consoleSelector = UIComboBox(this, 5, 0, 230, 14, 0,
		(GetListLen)&UIBootMenu::ConsoleGetLen,
		(GetListElement)&UIBootMenu::ConsoleGetElement);

	UICheckBox logoCheckBox = UICheckBox(this, 5, 0, 14, 14, "Show Tux logo");

	char extra_cmd[201] = "";

	UITextBox extraCmdInput = UITextBox(this, 5, 0, 230, 14, extra_cmd, 200);

	UIButton bootButton = UIButton(this, 5, 0, 50, 14, "Boot", (ActOfClick)&UIBootMenu::ClickBoot);
	UIButton resetButton = UIButton(this, 5, 0, 100, 14, "Reset to deault", (ActOfClick)&UIBootMenu::ClickReset);

	UIMenuElement uiarray[7] =
	{											// Up, D, L, R,
		{(UIBase*)&(UIBootMenu::cpuSelector),		5, 1, 0, 0}, //0

		{(UIBase*)&(UIBootMenu::ramSpin),			0, 2, 1, 1}, //1

		{(UIBase*)&(UIBootMenu::consoleSelector),	1, 3, 2, 2}, //2

		{(UIBase*)&(UIBootMenu::logoCheckBox),		2, 4, 3, 3}, //3

		{(UIBase*)&(UIBootMenu::extraCmdInput),		3, 5, 4, 4}, //4

		{(UIBase*)&(UIBootMenu::bootButton),		4, 0, 6, 6}, //5
		{(UIBase*)&(UIBootMenu::resetButton),		4, 0, 5, 5}, //6
	};

public:
	UIBootMenu();;

	void Draw(unsigned short* buf) override;
private:
	void RightSoftkeyClick() override;

	int CpusGetLen();
	int getDefCpu();
	const char* CpusGetElement(int id);
	int getSelectedCpu();

	int CheckRamLimit(int val) {
		if (val < 1)
			val = 1;

		if (val != detected_ram_mb) {
			static char tmp[32] = "";

			sprintf(tmp, "from %d MB detected", detected_ram_mb);

			ramSpin.label = tmp;
		}
		else
			ramSpin.label = "";
			
		return val;
	}

	int ConsoleGetLen();
	const char* ConsoleGetElement(int id);

	void ClickBoot();
	void ClickReset();

	void LoadSetting();
	void SaveSetting();
};

