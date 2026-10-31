#include "vmsys.h"
#include "vmio.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "vm4res.h"
#include "vmres.h"
#include "string.h"
#include <cstdint>

#ifndef WIN32

int linux_ram = 0;
const char* cmdline = 0;


#define UART_BASE       ((volatile unsigned char *)0xA0080000)
#define UART_THR        (*(volatile unsigned char *)(UART_BASE + 0x00))
#define UART_DLL        (*(volatile unsigned char *)(UART_BASE + 0x00))
#define UART_DLM        (*(volatile unsigned char *)(UART_BASE + 0x04))
#define UART_IER        (*(volatile unsigned char *)(UART_BASE + 0x04))
#define UART_FCR        (*(volatile unsigned char *)(UART_BASE + 0x08))
#define UART_LCR        (*(volatile unsigned char *)(UART_BASE + 0x0C))
#define UART_MCR        (*(volatile unsigned char *)(UART_BASE + 0x10))
#define UART_LSR        (*(volatile unsigned char *)(UART_BASE + 0x14))
#define UART_HIGHSPEED  (*(volatile unsigned char *)(UART_BASE + 0x24))

#define MPU_BASE		0xA0540000

#define WDT_MODE (*(volatile unsigned short *)0xA0030000)

#define RAM_BASE        0xF0000000
#define ATAGS_ADDR      (RAM_BASE + 0x0100)
#define KERNEL_ADDR     (RAM_BASE + 0x8000)
#define MACH_TYPE_MTK   33333

#define ATAG_CORE       0x54410001
#define ATAG_MEM        0x54410002
#define ATAG_CMDLINE    0x54410009
#define ATAG_NONE       0x00000000

static inline void uputc(int c)
{
	while ((UART_LSR & 0x20) == 0);
	UART_THR = c;
}

static inline void uputs(const char *s)
{
	for (int i = 0; s[i]; ++i) {
		if (s[i] == '\n') uputc('\r');
		uputc(s[i]);
	}
}

static inline void setup_uart(void)
{
	volatile int delay;

	UART_HIGHSPEED = 0x00;
	UART_LCR = 0x83;
	UART_DLL = 0x07;
	UART_DLM = 0x00;
	UART_LCR = 0x03;
	UART_FCR = 0x47;
	UART_MCR = 0x03;
	UART_IER = 0x00;

	*(UART_BASE + 0x4C) = 0;
	*(UART_BASE + 0x44) = 0;
	*(UART_BASE + 0x48) = 0;
	*(UART_BASE + 0x20) = 0;

	UART_FCR = 0x07;

	for (delay = 0; delay < 255; delay++) {
	}
}

static inline void turn_off_mpu_protection() {
	volatile uint32_t* mpu = (volatile uint32_t*)MPU_BASE;

#define MPU_PROT_ATTR_MASK 0x000000E0
	for (int i = 0; i < 16; i++) {
		uint32_t current_val = mpu[i];
		uint32_t base_addr = current_val & 0xFFFFF000;

		if ((base_addr >> 24) != 0x10)
			if ((current_val & MPU_PROT_ATTR_MASK) != 0)
				mpu[i] = current_val & ~MPU_PROT_ATTR_MASK;
	}
}

static inline void mask_all_irq() {
	volatile uint32_t* irq_mask_set0 = (volatile uint32_t*)0xA0060020;
	volatile uint32_t* irq_mask_set1 = (volatile uint32_t*)0xA0060024;
	volatile uint32_t* irq_eoi0 = (volatile uint32_t*)0xA0060160;
	volatile uint32_t* irq_eoi1 = (volatile uint32_t*)0xA0060164;

	volatile uint32_t* adie_eoi = (volatile uint32_t*)0xA0760160;

	*irq_mask_set0 = 0xFFFFFFFF;
	*irq_mask_set1 = 0xFFFFFFFF;

	*irq_eoi0 = 0xFFFFFFFF;
	*irq_eoi1 = 0xFFFFFFFF;

	*adie_eoi = 0xFFFFFFFF;
}

static void setup_atags(void)
{
	unsigned int* ptr = (unsigned int*)ATAGS_ADDR;

	int cmdlen = 0;
	int words;
	char* dst;
	int i; 

	*ptr++ = 5;
	*ptr++ = ATAG_CORE;
	*ptr++ = 0;
	*ptr++ = 4096;
	*ptr++ = 0; 

	*ptr++ = 4;
	*ptr++ = ATAG_MEM;
	*ptr++ = linux_ram;
	*ptr++ = RAM_BASE;

	while (cmdline[cmdlen] != '\0') {
		cmdlen++;
	}
	cmdlen++;

	words = (cmdlen + 3) / 4;
	*ptr++ = 2 + words;
	*ptr++ = ATAG_CMDLINE;

	dst = (char*)ptr;
	for (i = 0; i < cmdlen; i++) {
		dst[i] = cmdline[i];
	}

	ptr += words;

	*ptr++ = 0;
	*ptr++ = ATAG_NONE;
}



static VMUINT8* g_kernel_src = 0;
static VMINT g_kernel_size = 0;

extern "C" void __attribute__((noinline, noclone)) run_linux_safely()
{
	void (*kernel_entry)(int zero, int arch, unsigned int params);

	memmove((void*)KERNEL_ADDR, g_kernel_src, g_kernel_size);

	setup_atags();

	kernel_entry = (void (*)(int, int, unsigned int))KERNEL_ADDR;
	kernel_entry(0, MACH_TYPE_MTK, (unsigned int)ATAGS_ADDR);
}

#endif // !WIN32

void boot(int ram, const char* cmd) {
#ifndef WIN32
	linux_ram = ram;
	cmdline = cmd;

	VMINT size = 0;
	//VMUINT8* res = vm_load_resource("Image", &size);

	int f = vm_file_open((VMWSTR)u"e:\\Image", MODE_READ, 1);
	if (f < 0) {
		vm_exit_app();
		return;
	}

	vm_file_getfilesize(f, (VMUINT*)&size);

	VMUINT8* res = (VMUINT8*)vm_malloc(size);
	if (!res) {
		vm_exit_app();
		return;
	}

	VMUINT rsize = 0;
	vm_file_read(f, res, size, &rsize);
	vm_file_close(f);


	g_kernel_src = res;
	g_kernel_size = size;

	__asm__ __volatile__(
		"mrs r0, cpsr\n\t"
		"orr r0, r0, #0xC0\n\t" 
		"msr cpsr_c, r0\n\t"
		::: "r0", "cc"
	);

	setup_uart();

	WDT_MODE = (WDT_MODE | 0x2200) & 0xFFFE; //Disable WDT

	turn_off_mpu_protection();

	mask_all_irq();

	__asm__ __volatile__(
		"ldr sp, =0xF0700000\n\t"
		"b run_linux_safely\n\t" 
	);

#endif // !WIN32
}

