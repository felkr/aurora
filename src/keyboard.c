#include <stdbool.h>
#include "keyboard.h"
#include "terminal.h"
#include "stdio.h"
#include "io.h"
#include "serial.h"
#include "string.h"
#include "idts.h"
#include "fs/ff14b/source/ff.h"
#include "cpu.h"
#include "mem.h"

char layout[60] = "1234567890-=<>qwertyuiop[]?\\asdfghjkl;'!@#zxcvbnm,./";
char buffer[200] = "";
bool shiftKey = false;
bool ctrlKey = false;
enum KeyState keyMatrix[64] = {false};
enum KeyState prevMatrix[64] = {false};
void execute(char *input)
{
	if (input == NULL || input[0] == '\0')
	{
		return;
	}
	char *argv[100];
	char *reserve;
	uint8_t argc = 0;
	argv[argc] = strtok_r(input, " ", &reserve);
	while (argv[argc] != NULL)
	{
		argv[++argc] = strtok_r(NULL, " ", &reserve);
	}
	char *cmd = argv[0];
	if (strcmp("test", cmd) == 0)
	{
		kprint("it works!");
	}
	else if (strcmp("time", cmd) == 0)
	{
		while (1)
		{
		}
	}
	else if (strcmp("int", cmd) == 0)
	{
		// char c[20];
		// itoa(IDT[0].offset_higherbits, c, 10);
		// serial_println(c);
		asm volatile("int $0x80"
					 :
					 : "a"(0), "b"('a'));
	}
	else if (strcmp("shutdown", cmd) == 0)
	{
		// Initiate a shutdown, trying methods for several platform
		outw(0x604, 0x2000);  // QEMU
		outw(0xB004, 0x2000); // Bochs, old QEMU
		outw(0x4004, 0x3400); // Virtualbox
		shutdown();			  // Try to shutdown via realmode
	}
	else if (strcmp("cls", cmd) == 0)
	{
		terminal_clear();
	}
	else if (strcmp("l", cmd) == 0)
	{
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("Lorem ipsum dolor sit amet, consetetur sadipscing elitr,");
		kprintln("sed diam nonumy eirmod tempor invidunt ut labore et d");
	}
	else if (strcmp("ls", cmd) == 0)
	{
		DIR dir;
		FRESULT res = f_opendir(&dir, "/");
		if (res != FR_OK)
		{
			kprintf("Error opening directory: %d\n", res);
			return;
		}
		FILINFO fileInfo;
		while (f_readdir(&dir, &fileInfo) == FR_OK && fileInfo.fname[0] != '\0')
		{
			kprint(fileInfo.fname);
			kprint("\n");
		}
	}
	else if (strcmp("cat", cmd) == 0)
	{
		FRESULT res;

		FIL *fp = alloc_block();
		res = f_open(fp, argv[1], FA_READ);
		if (res != FR_OK)
		{
			serial_println("Failed to open file");
		}
		else
		{
			serial_println("File opened");
		}
		char buf[512] = {0};
		FRESULT rs = f_read(fp, buf, 512, NULL);
		kprintln(buf);
		f_close(fp);
	}
	else
	{
		FRESULT res;

		FIL *fp = alloc_block();
		res = f_open(fp, argv[0], FA_READ);
		if (res != FR_OK)
		{
			serial_println("Failed to open file");
			return;
		}
		else
		{
			serial_println("File opened");
		}
		char *buf = malloc(7 * 1024);
		// char buf[7 * 1024] = {0};
		FRESULT rs = f_read(fp, buf, 7 * 1024, NULL);
		if (rs != FR_OK)
		{
			serial_println("Failed to read file");
			return;
		}
		// for (size_t i = 0; i < 7 * 1024; i++)
		// {
		// 	serial_printf("0x%x ", buf[i]);
		// }

		launch_elf(buf);
		f_close(fp);
		// free(buf);
		// char msg[50] = "Unknown command ";
		// strcat(msg, cmd);
		// kprintln(msg);
	}
}

void irq1_handler(void)
{
	uint8_t key = inb(0x60);
	if ((key >> 7) > 0)
	{
		keyMatrix[(key & 0x7F) - 0x02] = Released;
	}
	else
	{
		keyMatrix[(key & 0x7F) - 0x02] = Pressed;
	}
	if (key == 0x2a)
		shiftKey = true;
	if (key == 0xaa)
		shiftKey = false;

	if (key == 0x1d)
		ctrlKey = true;
	if (key == 0x9d)
		ctrlKey = false;

	for (size_t i = 0; i < 60; i++)
	{
		if (layout[i] == 's' && keyMatrix[i] == Pressed && keyMatrix[i] != prevMatrix[i] && ctrlKey)
		{
			terminal_scrolldown(1);
			// break;
		}
		if (layout[i] == 'w' && keyMatrix[i] == Pressed && keyMatrix[i] != prevMatrix[i] && ctrlKey)
		{
			terminal_scrollup(1);
			// break;
		}
		if (layout[i] == '<' && keyMatrix[i] == Pressed && keyMatrix[i] != prevMatrix[i])
		{
			if (terminal_column > strlen(PROMPT))
			{
				buffer[strlen(buffer) - 1] = 0;
				terminal_column--;
				// kprint(" ");
				scrollback_putc(0);
				terminal_column--;
			}
		}
		if (layout[i] == '?' && keyMatrix[i] == Pressed && keyMatrix[i] != prevMatrix[i])
		{
			terminal_row++;
			terminal_column = 0;
			execute(buffer);
			*buffer = 0;
			kprintln("");
			kprint(PROMPT);
		}
		if (keyMatrix[i] != prevMatrix[i] && keyMatrix[i] == Pressed && layout[i] != '@' && layout[i] != '?' && layout[i] != '<' && !ctrlKey)
		{
			char c = layout[i];
			if (i == 55)
			{
				c = ' ';
			}
			if (shiftKey)
			{
				// c &= 0xdf;
				if (c >= 'a' && c <= 'z')
				{
					c -= 32;
				}
				else if (c >= 'A' && c <= 'Z')
				{
					c += 32;
				}
				else
				{
					switch (c)
					{
					case '-':
						c = '_';
						break;
					}
				}
			}
			kputc(c);
			append(buffer, c);
		}
	}
	for (size_t i = 0; i < 60; i++)
		prevMatrix[i] = keyMatrix[i];
	outb(0x20, 0x20); // EOI
}