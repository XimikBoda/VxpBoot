#include "UIBootMenu.h"
#include "Render.h"
#include "UIEngine.h"
#include "vmstdlib.h"
#include "vmio.h"
#include "string.h"
#include <vmbtcm.h>

extern UIEngine uiengine;

const char* get_cpu();
uint32_t get_ram();

UIBootMenu::UIBootMenu() {
	uiarray_internal = uiarray;
	uiarray_size_internal = sizeof(uiarray) / sizeof(uiarray[0]);

	addRamButton.x = 5;
	subtractRamButton.x = addRamButton.x + addRamButton.w + 2;
	ramInput.x = subtractRamButton.x + subtractRamButton.w + 5;


	addTuxButton.x = 5;
	subtractTuxButton.x = addTuxButton.x + addTuxButton.w + 2;
	tuxInput.x = subtractTuxButton.x + subtractTuxButton.w + 5;

	strcpy(detected_cpu, get_cpu());
	if(detected_cpu[0] == 'M' && detected_cpu[1] == 'T')
		sscanf(detected_cpu + 2, "%d", &detected_cpu_num);
	
	cpuSelector.cur_id = getDefCpu();

	detected_ram = get_ram();
	detected_ram_mb = detected_ram / 1024 / 1024;
	ram_mb = detected_ram_mb;
	UpdateRamStr();

	UpdateTuxStr();
}

void UIBootMenu::Draw(unsigned short* buf)
{
	Render::draw_fill_rect(buf, 0, 0, screen_w, screen_h, 0x0000);

	int y = 3;
	int x = 5;

	Render::draw_text_white_centered(buf, 0, y, screen_w, "VxpBoot");

	Render::draw_fill_rect(buf, 0, y + 13, screen_w, 1, 0xFFFF);

	y += 20;
	Render::draw_text_white(buf, 5, y, "Cpu:");
	y += 15;

	cpuSelector.y = y;

	y += 20;
	Render::draw_text_white(buf, 5, y, "Ram size:");
	y += 15;

	addRamButton.y = y;
	subtractRamButton.y = y;
	ramInput.y = y;
	{
		char tmp[100] = "MB";
		if(ramInput.isFocused)
			sscanf(ram_str, "%d", &ram_mb);
		if (ram_mb != detected_ram_mb)
			sprintf(tmp, "MB from %d MB detected", detected_ram_mb);
		Render::draw_text_white(buf, 5 + ramInput.x + ramInput.w, y + 3, tmp);
	}

	y += 20;
	Render::draw_text_white(buf, 5, y, "Tux count (don`t ask):");
	y += 15;

	addTuxButton.y = y;
	subtractTuxButton.y = y;
	tuxInput.y = y;

	if (tuxInput.isFocused)
		sscanf(tux_count_str, "%d", &tux_count);
		

	y += 20;
	Render::draw_text_white(buf, 5, y, "Console:");
	y += 15;

	consoleSelector.y = y;

	y += 20;
	Render::draw_text_white(buf, 5, y, "Extra cmdline:");
	y += 15;

	extraCmdInput.y = y;

	y += 20;
	bootButton.y = y;

	for (int i = 0; i < uiarray_size_internal; ++i)
		uiarray[i].el->QDraw(buf);
}

void UIBootMenu::RightSoftkeyClick() { 
	//uiengine.PopUI(); 
}

int UIBootMenu::CpusGetLen() {
	switch (detected_cpu_num) {
	case 6250:
	case 6260:
	case 6261:
		return 3;
	}
	return 4;
}

int UIBootMenu::getDefCpu() {
	switch (detected_cpu_num) {
	case 6250:
		return 0;
	case 6260:
		return 1;
	case 6261:
		return 2;
	default:
		return 3;
	}
}

const char* UIBootMenu::CpusGetElement(int id)
{
	switch (id) {
	case 0:
		return detected_cpu_num == 6250 ? "MT6250 (Detected)" : "MT6250";
	case 1:
		return detected_cpu_num == 6260 ? "MT6260 (Detected)" : "MT6260";
	case 2:
		return detected_cpu_num == 6261 ? "MT6261 (Detected)" : "MT6261";
	}

	static char str[200] = "";
	sprintf(str, "Unsuported (%s)", detected_cpu);

	return str;
}

void UIBootMenu::UpdateRamStr() {
	sprintf(ram_str, "%d", ram_mb);
}

void UIBootMenu::ClickAddRam() {
	ram_mb++;
	UpdateRamStr();

}

void UIBootMenu::ClickSubtractRam() {
	if (ram_mb > 0)
		ram_mb--;
	UpdateRamStr();
}

void UIBootMenu::UpdateTuxStr() {
	sprintf(tux_count_str, "%d", tux_count);
}

void UIBootMenu::ClickAddTux() {
	tux_count++;
	UpdateTuxStr();

}

void UIBootMenu::ClickSubtractTux() {
	if (tux_count > 0)
		tux_count--;
	UpdateTuxStr();
}

int UIBootMenu::ConsoleGetLen() {
	return sizeof(console_modes) / sizeof(console_modes[0]);
}

const char* UIBootMenu::ConsoleGetElement(int id) {
	return console_modes[id].name;
}

static char cmdline[500] = "";
void UIBootMenu::ClickBoot() {
	sprintf(cmdline, "%s fbcon=map:0,font:6x11,logo-count:%d %s %s ", 
		console_modes[consoleSelector.cur_id].cmd, 
		tux_count, (tux_count == 0 ? "logo.nologo" : ""), 
		extra_cmd);

	void boot(int ram, const char* cmd);

	boot(ram_mb * 1024 * 1024, cmdline);
}
