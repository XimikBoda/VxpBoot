#include "vmsys.h"
#include "vmio.h"
#include "vmchset.h"
#include "vmgraph.h"
#include "vmstdlib.h"
#include "vm4res.h"
#include "vmres.h"
#include "string.h"
#include <cstdint>

#include "T2Input.h"
#include "UIEngine.h"
#include "UIBootMenu.h"
#include "Render.h"

#include "ProFont6x11.h"

VMINT		layer_hdl[2];	// layer handle array. 
VMUINT8* layer_buf[2] = {};

VMINT screen_w = 0;
VMINT screen_h = 0;

T2Input t2input;
UIEngine uiengine;

UIBootMenu* uib = 0;

extern "C" void flush_layer() {
	vm_graphic_flush_layer(layer_hdl, 2);
	vm_graphic_fill_rect(layer_buf[1], 0, 0, screen_w, screen_h, tr_color, tr_color);
}

void handle_keyevt(VMINT event, VMINT keycode) {
	uiengine.KeyboardEvent(event, keycode);
	t2input.draw();
	flush_layer();
}

void text_event(const char* str) {
	uiengine.TextEvent(str);
	t2input.draw();
	flush_layer();
}

void handle_penevt(int event, int x, int y) {
	t2input.handle_penevt(event, x, y);
	t2input.draw();
	flush_layer();
}

static bool is_digit(char ch) {
	return ch >= '0' && ch <= '9';
}

const char* get_cpu() {
#ifdef WIN32
	char host_version[100] = "MRE_3100;240x320;MT6250;some";
#else
	char host_version[100] = "";
	vm_get_sys_property(MRE_SYS_HOST_VERSION, host_version, 100);
#endif // WIN32

	static char cpu[100] = "";
	{
		int i = 0;
		for(char* token = strtok(host_version, ";"); token; token = strtok(NULL, ";")) {
			if (i == 2) {
				strcpy(cpu, token);
				break;
			}
			++i;
		}
	}

	return cpu;
}

uint32_t get_ram() {
#ifdef WIN32
	return 8 * 1024 * 1024;
#endif // WIN32

	volatile uint32_t* mpu_base = (volatile uint32_t*)0xA0540000;

	for (int i = 0; i < 16; i++)
		if ((mpu_base[i] & 0xFFFFF000) == 0xF0008000)
			return mpu_base[i + 16] & 0x0FFFFFFF;

	return 0;
}

void vm_main(void) {
	layer_hdl[0] = -1;
	screen_w = vm_graphic_get_screen_width();
	screen_h = vm_graphic_get_screen_height();

	layer_hdl[0] = vm_graphic_create_layer(0, 0, screen_w, screen_h, -1);
	layer_buf[0] = vm_graphic_get_layer_buffer(layer_hdl[0]);
	layer_hdl[1] = vm_graphic_create_layer(0, 0, screen_w, screen_h, tr_color);
	layer_buf[1] = vm_graphic_get_layer_buffer(layer_hdl[1]);
	vm_graphic_set_clip(0, 0, screen_w, screen_h);

	vm_graphic_fill_rect(layer_buf[1], 0, 0, screen_w, screen_h, tr_color, tr_color);
	vm_graphic_fill_rect(layer_buf[0], 0, 0, screen_w, screen_h, 0, 0);

	uib = new UIBootMenu();

	t2input.init();
	uiengine.init();

	uiengine.def_buf = (unsigned short*)layer_buf[0];
	t2input.scr_buf = (unsigned char*)layer_buf[1];
	t2input.layer_handle = layer_hdl[1];

	uiengine.PushUI(uib);

	//vm_reg_sysevt_callback(handle_sysevt);
	vm_reg_keyboard_callback(handle_keyevt);
	vm_reg_pen_callback(handle_penevt);


	//macImput->

	flush_layer();
}


void* operator new(size_t size) {
	void* p = vm_malloc(size);
	return p;
}

void operator delete(void* p) {
	vm_free(p);
}