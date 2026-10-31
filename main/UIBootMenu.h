#pragma once
#include "UIBase.h"
#include "UIMenuBase.h"
#include "UIButton.h"
#include "UITextBox.h"
#include "UIComboBox.h"
#include "UICheckBox.h"
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

	int ram_mb = 0;
	char ram_str[21] = "";

	UIButton addRamButton = UIButton(this, 5, 0, 14, 14, "+", (ActOfClick)&UIBootMenu::ClickAddRam);
	UIButton subtractRamButton = UIButton(this, 5, 0, 14, 14, "-", (ActOfClick)&UIBootMenu::ClickSubtractRam);
	UITextBox ramInput = UITextBox(this, 5, 0, 30, 14, ram_str, 20);

	UIComboBox consoleSelector = UIComboBox(this, 5, 0, 230, 14, 0,
		(GetListLen)&UIBootMenu::ConsoleGetLen,
		(GetListElement)&UIBootMenu::ConsoleGetElement);

	UICheckBox logoCheckBox = UICheckBox(this, 5, 0, 14, 14, "Show Tux logo");

	char extra_cmd[201] = "";

	UITextBox extraCmdInput = UITextBox(this, 5, 0, 230, 14, extra_cmd, 200);

	UIButton bootButton = UIButton(this, 5, 0, 50, 14, "Boot", (ActOfClick)&UIBootMenu::ClickBoot);

	UIMenuElement uiarray[8] =
	{
		{(UIBase*)&(UIBootMenu::cpuSelector),		7, 1, 0, 0}, //0

		{(UIBase*)&(UIBootMenu::addRamButton),		0, 4, 3, 2}, //1
		{(UIBase*)&(UIBootMenu::subtractRamButton),	0, 4, 1, 3}, //2
		{(UIBase*)&(UIBootMenu::ramInput),			0, 4, 2, 1}, //3

		{(UIBase*)&(UIBootMenu::consoleSelector),	1, 5, 4, 4}, //4

		{(UIBase*)&(UIBootMenu::logoCheckBox),		4, 6, 5, 5}, //5

		{(UIBase*)&(UIBootMenu::extraCmdInput),		5, 7, 6, 6}, //6

		{(UIBase*)&(UIBootMenu::bootButton),		6, 0, 7, 7}, //7
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

	void UpdateRamStr();
	void ClickAddRam();
	void ClickSubtractRam();

	int ConsoleGetLen();
	const char* ConsoleGetElement(int id);

	void ClickBoot();
};

