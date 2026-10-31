#pragma once
#include "UIBase.h"
#include "UIMenuBase.h"
#include "UIButton.h"
#include "UITextBox.h"
#include "UIComboBox.h"
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

	int tux_count = 1;
	char tux_count_str[21] = "";

	UIButton addTuxButton = UIButton(this, 5, 0, 14, 14, "+", (ActOfClick)&UIBootMenu::ClickAddTux);
	UIButton subtractTuxButton = UIButton(this, 5, 0, 14, 14, "-", (ActOfClick)&UIBootMenu::ClickSubtractTux);
	UITextBox tuxInput = UITextBox(this, 5, 0, 30, 14, tux_count_str, 20);

	UIComboBox consoleSelector = UIComboBox(this, 5, 0, 230, 14, 0,
		(GetListLen)&UIBootMenu::ConsoleGetLen,
		(GetListElement)&UIBootMenu::ConsoleGetElement);

	char extra_cmd[201] = "";

	UITextBox extraCmdInput = UITextBox(this, 5, 0, 230, 14, extra_cmd, 200);

	UIButton bootButton = UIButton(this, 5, 0, 50, 14, "Boot", (ActOfClick)&UIBootMenu::ClickBoot);

	UIMenuElement uiarray[10] =
	{
		{(UIBase*)&(UIBootMenu::cpuSelector),		9, 1, 0, 0}, //0

		{(UIBase*)&(UIBootMenu::addRamButton),		0, 4, 3, 2}, //1
		{(UIBase*)&(UIBootMenu::subtractRamButton),	0, 4, 1, 3}, //2
		{(UIBase*)&(UIBootMenu::ramInput),			0, 4, 2, 1}, //3

		{(UIBase*)&(UIBootMenu::addTuxButton),		1, 7, 6, 5}, //4
		{(UIBase*)&(UIBootMenu::subtractTuxButton),	1, 7, 4, 6}, //5
		{(UIBase*)&(UIBootMenu::tuxInput),			1, 7, 5, 4}, //6

		{(UIBase*)&(UIBootMenu::consoleSelector),	4, 8, 7, 7}, //7

		{(UIBase*)&(UIBootMenu::extraCmdInput),		7, 9, 8, 8}, //8

		{(UIBase*)&(UIBootMenu::bootButton),		8, 0, 9, 9}, //9
	};

public:
	UIBootMenu();;

	void Draw(unsigned short* buf) override;
private:
	void RightSoftkeyClick() override;


	int CpusGetLen();
	int getDefCpu();
	const char* CpusGetElement(int id);

	void UpdateRamStr();
	void ClickAddRam();
	void ClickSubtractRam();

	void UpdateTuxStr();
	void ClickAddTux();
	void ClickSubtractTux();

	int ConsoleGetLen();
	const char* ConsoleGetElement(int id);

	void ClickBoot();
};

