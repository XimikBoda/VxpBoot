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

	UISpinBox ramSpin = UISpinBox(this, 5, 0, 60, 14, "%d MB", "", (CheckLimit)&UIBootMenu::CheckRamLimit);

	UIComboBox consoleSelector = UIComboBox(this, 5, 0, 230, 14, 0,
		(GetListLen)&UIBootMenu::ConsoleGetLen,
		(GetListElement)&UIBootMenu::ConsoleGetElement);


	UISpinBox fpsSpin = UISpinBox(this, 5, 0, 60, 14, "%d fps", "", (CheckLimit)&UIBootMenu::CheckFpsLimit);

	UICheckBox logoCheckBox = UICheckBox(this, 5, 0, 14, 14, "Show Tux logo");

	char extra_cmd[201] = "";

	UITextBox extraCmdInput = UITextBox(this, 5, 0, 230, 14, extra_cmd, 200);

	UIButton bootButton = UIButton(this, 5, 0, 50, 14, "Boot", (ActOfClick)&UIBootMenu::ClickBoot);
	UIButton resetButton = UIButton(this, 5, 0, 100, 14, "Reset to default", (ActOfClick)&UIBootMenu::ClickReset);

	UIMenuElement uiarray[8] =
	{											// Up, D, L, R,
		{(UIBase*)&(UIBootMenu::cpuSelector),		6, 1, 0, 0}, //0

		{(UIBase*)&(UIBootMenu::ramSpin),			0, 2, 1, 1}, //1

		{(UIBase*)&(UIBootMenu::consoleSelector),	1, 3, 2, 2}, //2

		{(UIBase*)&(UIBootMenu::fpsSpin),			2, 4, 3, 3}, //3

		{(UIBase*)&(UIBootMenu::logoCheckBox),		3, 5, 4, 4}, //4

		{(UIBase*)&(UIBootMenu::extraCmdInput),		4, 6, 5, 5}, //5

		{(UIBase*)&(UIBootMenu::bootButton),		5, 0, 7, 7}, //6
		{(UIBase*)&(UIBootMenu::resetButton),		5, 0, 6, 6}, //7
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

	int CheckRamLimit(int val);
	int CheckFpsLimit(int val);

	int ConsoleGetLen();
	const char* ConsoleGetElement(int id);

	void ClickBoot();
	void ClickReset();

	void LoadSetting();
	void SaveSetting();
};

