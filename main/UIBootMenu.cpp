#include "UIBootMenu.h"
#include "UI/Render.h"
#include "UI/UIEngine.h"
#include "vmstdlib.h"
#include "vmio.h"
#include "string.h"
#include <vmbtcm.h>

extern UIEngine uiengine;

const VMWSTR save_file_path = (VMWSTR)u"e:\\VxpBoot.cfg";

const char* get_cpu();
const char* get_model_name();
uint32_t get_ram();

UIBootMenu::UIBootMenu() {
	uiarray_internal = uiarray;
	uiarray_size_internal = sizeof(uiarray) / sizeof(uiarray[0]);

	resetButton.x = bootButton.x + bootButton.w + 5;

	strcpy(detected_cpu, get_cpu());
	if(detected_cpu[0] == 'M' && detected_cpu[1] == 'T')
		sscanf(detected_cpu + 2, "%d", &detected_cpu_num);

	detected_ram = get_ram();
	detected_ram_mb = detected_ram / 1024 / 1024;

	ClickReset();
	LoadSetting();
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

	ramSpin.y = y;

	y += 20;
	Render::draw_text_white(buf, 5, y, "Console:");
	y += 15;

	consoleSelector.y = y;

	y += 20;
	Render::draw_text_white(buf, 5, y, "Screen:");
	y += 15;

	fpsSpin.y = y;

	y += 20;
	logoCheckBox.y = y;

	y += 20;
	Render::draw_text_white(buf, 5, y, "Extra cmdline:");
	y += 15;

	extraCmdInput.y = y;

	y += 20;
	bootButton.y = y;
	resetButton.y = y;

	for (int i = 0; i < uiarray_size_internal; ++i)
		uiarray[i].el->QDraw(buf);
}

void UIBootMenu::RightSoftkeyClick() {
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

int UIBootMenu::getSelectedCpu() {
	switch (cpuSelector.cur_id) {
	case 0:
		return 6250;
	case 1:
		return 6260;
	case 2:
		return 6261;
	case 3: 
		return detected_cpu_num;
	}
}

int UIBootMenu::CheckRamLimit(int val) {
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

int UIBootMenu::CheckFpsLimit(int val) {
	if (val < 1)
		val = 1;

	if (val > 30) 
		fpsSpin.label = "> 30 can be unstable";
	else
		fpsSpin.label = "";

	return val;
}

int UIBootMenu::ConsoleGetLen() {
	return sizeof(console_modes) / sizeof(console_modes[0]);
}

const char* UIBootMenu::ConsoleGetElement(int id) {
	return console_modes[id].name;
}

static char cmdline[500] = "";
void UIBootMenu::ClickBoot() {
	SaveSetting();

	sprintf(cmdline, 
		"%s " // console 
		"fbcon=map:0,font:6x11 %s " // fbcon
		"mtk_cpu_name=MT%04d device_model=%s " // names
		"mt62xx_fb.width=%d mt62xx_fb.height=%d mt62xx_fb.fps=%d " //screen
		"%s ", // extra cmdline
		console_modes[consoleSelector.cur_id].cmd,
		(!logoCheckBox.value ? "logo.nologo" : ""),
		getSelectedCpu(), get_model_name(),
		vm_graphic_get_screen_width(), vm_graphic_get_screen_height(), fpsSpin.value,
		extra_cmd);

	void boot(int ram, const char* cmd, int cpu);

	boot(ramSpin.value * 1024 * 1024, cmdline, getSelectedCpu());
}

void UIBootMenu::ClickReset() {
	//CPU
	cpuSelector.cur_id = getDefCpu();

	//RAM
	ramSpin.value = detected_ram_mb;

	//Console
	consoleSelector.cur_id = 0;

	//Screen
	fpsSpin.value = 15;

	logoCheckBox.value = true;

	// Cmdline	
	extra_cmd[0] = '\0';
}

void UIBootMenu::LoadSetting() {
	char tmp[512] = "";
	int h = vm_file_open(save_file_path, MODE_READ, 1);
	if (h < 0) return;

	VMUINT readed = 0, size = 0;
	vm_file_getfilesize(h, &size);

	if (size >= sizeof(tmp))
		size = sizeof(tmp) - 1;

	vm_file_read(h, tmp, size, &readed);
	vm_file_close(h);

	tmp[readed] = '\0';

	int scaned = sscanf(tmp, "%d %d\n%d\n%d %d\n%200[^\n]",
		&cpuSelector.cur_id, &ramSpin.value,
		&consoleSelector.cur_id, 
		&fpsSpin.value, &logoCheckBox.value,
		extra_cmd);
}

void UIBootMenu::SaveSetting() {
	char tmp[512] = "";
	sprintf(tmp, "%d %d\n%d\n%d %d\n%s\n", 
		cpuSelector.cur_id, ramSpin.value,
		consoleSelector.cur_id, 
		fpsSpin.value, logoCheckBox.value,
		extra_cmd);
	
	int h = vm_file_open(save_file_path, MODE_CREATE_ALWAYS_WRITE, 1);
	if (h < 0) return;
	VMUINT writen = 0;
	vm_file_write(h, tmp, strlen(tmp), &writen);
	vm_file_close(h);
}