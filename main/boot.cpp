#include "vmsys.h"
#include "vmio.h"
#include "vmchset.h"
#include "vmstdlib.h"
#include "vm4res.h"
#include "vmres.h"
#include "string.h"
#include <cstdint>

int linux_ram = 0;
const char* cmdline = 0;


#define UART_BASE       0xA0080000
#define UART_THR        (*(volatile unsigned char *)(UART_BASE + 0x00))
#define UART_DLL        (*(volatile unsigned char *)(UART_BASE + 0x00))
#define UART_DLM        (*(volatile unsigned char *)(UART_BASE + 0x04))
#define UART_IER        (*(volatile unsigned char *)(UART_BASE + 0x04))
#define UART_FCR        (*(volatile unsigned char *)(UART_BASE + 0x08))
#define UART_LCR        (*(volatile unsigned char *)(UART_BASE + 0x0C))
#define UART_MCR        (*(volatile unsigned char *)(UART_BASE + 0x10))
#define UART_LSR        (*(volatile unsigned char *)(UART_BASE + 0x14))
#define UART_HIGHSPEED  (*(volatile unsigned char *)(UART_BASE + 0x24))

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

static void setup_atags(void)
{
	unsigned int* ptr = (unsigned int*)ATAGS_ADDR;

	//const char* cmdline = "console=ttyS0,115200n8 debug user_debug=31 loglevel=8"; //earlyprintk keep_bootcon dhash_entries=1024 ihash_entries=1024 mhash_entries=1024 mphash_entries=1024
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

	uputs("Copying Image to 0xF0008000...\n"); 
	memmove((void*)KERNEL_ADDR, g_kernel_src, g_kernel_size);
	uputs("Kernel copied successfully!\n");

	unsigned char* k_ptr = (unsigned char*)KERNEL_ADDR;
	VMCHAR hex_buf[100];

	uputs("Kernel start hex dump:\n");
	sprintf(hex_buf, "%02X %02X %02X %02X %02X %02X %02X %02X\n",
		k_ptr[0], k_ptr[1], k_ptr[2], k_ptr[3],
		k_ptr[4], k_ptr[5], k_ptr[6], k_ptr[7]);
	uputs(hex_buf);

	sprintf(hex_buf, "%02X %02X %02X %02X %02X %02X %02X %02X\n",
		k_ptr[8], k_ptr[9], k_ptr[10], k_ptr[11],
		k_ptr[12], k_ptr[13], k_ptr[14], k_ptr[15]);
	uputs(hex_buf);


	uputs("setup_atags\n");

	setup_atags();


	uputs("kernel_entry\n");

	kernel_entry = (void (*)(int, int, unsigned int))KERNEL_ADDR;
	kernel_entry(0, MACH_TYPE_MTK, (unsigned int)ATAGS_ADDR);
}

static inline void arch_decomp_setup(void)
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

	for (delay = 0; delay < 255; delay++) {
	}
}


void boot(int ram, const char* cmd) {
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

	arch_decomp_setup();

	WDT_MODE = (WDT_MODE | 0x2200) & 0xFFFE;

	VMCHAR s[100];
	sprintf(s, "\n\rI at  0x%08X, stack at  0x%08X\n", vm_main, s);
	uputs(s);

	{
		volatile uint32_t* mpu = (volatile uint32_t*)0xA0540000;
		volatile uint32_t* zero_page = (volatile uint32_t*)0x00000000;
		char dbg_buf[64];

		uputs("\n=== MPU STATE DUMP ===\n");

		uputs("--- Start Addresses & Flags ---\n");
		for (int i = 0; i < 16; i++) {
			sprintf(dbg_buf, "MPU[0x%02X] = 0x%08X\n", i * 4, mpu[i]);
			uputs(dbg_buf);
		}

		uputs("--- End Addresses ---\n");
		for (int i = 16; i < 32; i++) {
			sprintf(dbg_buf, "MPU[0x%02X] = 0x%08X\n", i * 4, mpu[i]);
			uputs(dbg_buf);
		}

		uputs("\n=== MPU UNLOCK (BASE ONLY) ===\n");

		for (int i = 0; i < 16; i++) {
			#define MPU_PROT_ATTR_MASK 0x000000E0
			uint32_t current_val = mpu[i];
			uint32_t base_addr = current_val & 0xFFFFF000;

			if ((base_addr >> 24) != 0x10) {

				if ((current_val & MPU_PROT_ATTR_MASK) != 0) {
					sprintf(dbg_buf, "Unlocking region %d (Base: 0x%08X, Flag: 0x%03X)\n",
						i, base_addr, current_val & 0xFFF);
					uputs(dbg_buf);

					mpu[i] = current_val & ~MPU_PROT_ATTR_MASK;

					sprintf(dbg_buf, "Patched to: 0x%08X\n", mpu[i]);
					uputs(dbg_buf);
				}
			}
		}

	}

	{
		volatile uint32_t* irq_mask_set0 = (volatile uint32_t*)0xA0060020;
		volatile uint32_t* irq_mask_set1 = (volatile uint32_t*)0xA0060024;
		volatile uint32_t* irq_eoi0 = (volatile uint32_t*)0xA0060160;
		volatile uint32_t* irq_eoi1 = (volatile uint32_t*)0xA0060164;

		volatile uint32_t* adie_eoi = (volatile uint32_t*)0xA0760160;

		uputs("\n=== SILENCING HARDWARE INTERRUPTS ===\n");

		*irq_mask_set0 = 0xFFFFFFFF;
		*irq_mask_set1 = 0xFFFFFFFF;

		*irq_eoi0 = 0xFFFFFFFF;
		*irq_eoi1 = 0xFFFFFFFF;

		*adie_eoi = 0xFFFFFFFF;

		uputs("All hardware interrupts masked and cleared.\n");
		uputs("=========================================\n\n");
	}


	__asm__ __volatile__(
		"ldr sp, =0xF0700000\n\t"
		"b run_linux_safely\n\t" 
	);

}

