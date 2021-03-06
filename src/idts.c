#include "io.h"
#include "idts.h"
#include "stdio.h"
#include "serial.h"
#include "cpu.h"
#include "string.h"
#include "gdt.h"
#include "virtmem.h"

struct IDT_entry IDT[256];

#define IDT_FLAG_INTERRUPT_GATE 0xe
#define IDT_FLAG_PRESENT 0x80
#define IDT_FLAG_RING0 0x00
#define IDT_FLAG_RING3 0x60

#define REGISTER_IRQ(irq, handler)                                               \
    IDT[irq].offset_lowerbits = (uint16_t)((uint32_t)handler & 0xFFFF);          \
    IDT[irq].offset_higherbits = (uint16_t)(((uint32_t)handler >> 16) & 0xFFFF); \
    IDT[irq].selector = 0x08;                                                    \
    IDT[irq].zero = 0;                                                           \
    IDT[irq].type_attr = IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_PRESENT | IDT_FLAG_RING0;

void idt_init(void)
{
    extern int load_idt();
    extern int irq0();
    extern int irq1();
    extern int irq2();
    extern int irq3();
    extern int irq4();
    extern int irq5();
    extern int irq6();
    extern int irq7();
    extern int irq8();
    extern int irq9();
    extern int irq10();
    extern int irq11();
    extern int irq12();
    extern int irq13();
    extern int irq14();
    extern int irq15();
    extern int generic_irq();
    extern int pf_irq();
    extern int irq80();

    unsigned long irq0_address;
    unsigned long irq1_address;
    unsigned long irq2_address;
    unsigned long irq3_address;
    unsigned long irq4_address;
    unsigned long irq5_address;
    unsigned long irq6_address;
    unsigned long irq7_address;
    unsigned long irq8_address;
    unsigned long irq9_address;
    unsigned long irq10_address;
    unsigned long irq11_address;
    unsigned long irq12_address;
    unsigned long irq13_address;
    unsigned long irq14_address;
    unsigned long irq15_address;
    unsigned long generic_irq_address;
    unsigned long pf_irq_address;
    unsigned long irq80_address;
    unsigned long idt_address;
    unsigned long idt_ptr[2];

    // Master-PIC initialisieren
    outb(0x20, 0x11); // Initialisierungsbefehl fuer den PIC
    outb(0x21, 0x20); // Interruptnummer fuer IRQ 0
    outb(0x21, 0x04); // An IRQ 2 haengt der Slave
    outb(0x21, 0x01); // ICW 4

    // Slave-PIC initialisieren
    outb(0xa0, 0x11); // Initialisierungsbefehl fuer den PIC
    outb(0xa1, 0x28); // Interruptnummer fuer IRQ 8
    outb(0xa1, 0x02); // An IRQ 2 haengt der Slave
    outb(0xa1, 0x01); // ICW 4

    // Alle IRQs aktivieren (demaskieren)
    outb(0x20, 0x0);
    outb(0xa0, 0x0);

    irq0_address = (unsigned long)irq0;
    IDT[32].offset_lowerbits = irq0_address & 0xffff;
    IDT[32].selector = 0x8; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[32].zero = 0;
    IDT[32].type_attr = IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT; /* INTERRUPT_GATE */
    IDT[32].offset_higherbits = (irq0_address & 0xffff0000) >> 16;

    irq1_address = (unsigned long)irq1;
    IDT[33].offset_lowerbits = irq1_address & 0xffff;
    IDT[33].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[33].zero = 0;
    IDT[33].type_attr = IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT; /* INTERRUPT_GATE */
    IDT[33].offset_higherbits = (irq1_address & 0xffff0000) >> 16;

    irq2_address = (unsigned long)irq2;
    IDT[34].offset_lowerbits = irq2_address & 0xffff;
    IDT[34].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[34].zero = 0;
    IDT[34].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[34].offset_higherbits = (irq2_address & 0xffff0000) >> 16;

    irq3_address = (unsigned long)irq3;
    IDT[35].offset_lowerbits = irq3_address & 0xffff;
    IDT[35].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[35].zero = 0;
    IDT[35].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[35].offset_higherbits = (irq3_address & 0xffff0000) >> 16;

    irq4_address = (unsigned long)irq4;
    IDT[36].offset_lowerbits = irq4_address & 0xffff;
    IDT[36].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[36].zero = 0;
    IDT[36].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[36].offset_higherbits = (irq4_address & 0xffff0000) >> 16;

    irq5_address = (unsigned long)irq5;
    IDT[37].offset_lowerbits = irq5_address & 0xffff;
    IDT[37].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[37].zero = 0;
    IDT[37].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[37].offset_higherbits = (irq5_address & 0xffff0000) >> 16;

    irq6_address = (unsigned long)irq6;
    IDT[38].offset_lowerbits = irq6_address & 0xffff;
    IDT[38].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[38].zero = 0;
    IDT[38].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[38].offset_higherbits = (irq6_address & 0xffff0000) >> 16;

    irq7_address = (unsigned long)irq7;
    IDT[39].offset_lowerbits = irq7_address & 0xffff;
    IDT[39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[39].zero = 0;
    IDT[39].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[39].offset_higherbits = (irq7_address & 0xffff0000) >> 16;

    irq8_address = (unsigned long)irq8;
    IDT[40].offset_lowerbits = irq8_address & 0xffff;
    IDT[40].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[40].zero = 0;
    IDT[40].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[40].offset_higherbits = (irq8_address & 0xffff0000) >> 16;

    irq9_address = (unsigned long)irq9;
    IDT[41].offset_lowerbits = irq9_address & 0xffff;
    IDT[41].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[41].zero = 0;
    IDT[41].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[41].offset_higherbits = (irq9_address & 0xffff0000) >> 16;

    irq10_address = (unsigned long)irq10;
    IDT[42].offset_lowerbits = irq10_address & 0xffff;
    IDT[42].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[42].zero = 0;
    IDT[42].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[42].offset_higherbits = (irq10_address & 0xffff0000) >> 16;

    irq11_address = (unsigned long)irq11;
    IDT[43].offset_lowerbits = irq11_address & 0xffff;
    IDT[43].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[43].zero = 0;
    IDT[43].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[43].offset_higherbits = (irq11_address & 0xffff0000) >> 16;

    irq12_address = (unsigned long)irq12;
    IDT[44].offset_lowerbits = irq12_address & 0xffff;
    IDT[44].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[44].zero = 0;
    IDT[44].type_attr = IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT; /* INTERRUPT_GATE */
    IDT[44].offset_higherbits = (irq12_address & 0xffff0000) >> 16;

    irq13_address = (unsigned long)irq13;
    IDT[45].offset_lowerbits = irq13_address & 0xffff;
    IDT[45].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[45].zero = 0;
    IDT[45].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[45].offset_higherbits = (irq13_address & 0xffff0000) >> 16;

    irq14_address = (unsigned long)irq14;
    IDT[46].offset_lowerbits = irq14_address & 0xffff;
    IDT[46].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[46].zero = 0;
    IDT[46].type_attr = IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT; /* INTERRUPT_GATE */
    IDT[46].offset_higherbits = (irq14_address & 0xffff0000) >> 16;

    irq15_address = (unsigned long)irq15;
    IDT[47].offset_lowerbits = irq15_address & 0xffff;
    IDT[47].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[47].zero = 0;
    IDT[47].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[47].offset_higherbits = (irq15_address & 0xffff0000) >> 16;

    generic_irq_address = (unsigned long)generic_irq;
    IDT[0x0d].offset_lowerbits = generic_irq_address & 0xffff;
    IDT[0x0d].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[0x0d].zero = 0;
    IDT[0x0d].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[0x0d].offset_higherbits = (generic_irq_address & 0xffff0000) >> 16;

    pf_irq_address = (unsigned long)pf_irq;
    IDT[0x0e].offset_lowerbits = pf_irq_address & 0xffff;
    IDT[0x0e].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[0x0e].zero = 0;
    IDT[0x0e].type_attr = 0x8e; /* INTERRUPT_GATE */
    IDT[0x0e].offset_higherbits = (pf_irq_address & 0xffff0000) >> 16;

    irq80_address = (unsigned long)irq80;
    IDT[0x80].offset_lowerbits = irq80_address & 0xffff;
    IDT[0x80].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
    IDT[0x80].zero = 0;
    IDT[0x80].type_attr = IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING3 | IDT_FLAG_PRESENT; /* INTERRUPT_GATE */
    IDT[0x80].offset_higherbits = (irq80_address & 0xffff0000) >> 16;

    /* fill the IDT descriptor */
    idt_address = (unsigned long)IDT;
    idt_ptr[0] = (sizeof(struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
    idt_ptr[1] = idt_address >> 16;

    load_idt(idt_ptr);

    outb(0x21, 0x0);
    outb(0xa1, 0x0);
}
static uint32_t tss[32] = {0, 0, 0x10};

struct cpu_state *irq0_handler(struct cpu_state *cpu)
{
    struct cpu_state *new_cpu = cpu;
    new_cpu = schedule(cpu);
    tss[1] = (uint32_t)(new_cpu + 1);
    set_entry(5, (uint32_t)tss, sizeof(tss), GDT_FLAG_TSS | GDT_FLAG_PRESENT | GDT_FLAG_RING3);
    asm volatile("ltr %%ax"
                 :
                 : "a"(5 << 3));
    if (cpu != new_cpu)
    {
        // vmem_activate_context(get_current_task()->context->pagedir);
        // serial_printf("Switch to context 0x%x\n", get_current_task()->cr3);
        // asm volatile("mov %0, %%cr3"
        //              :
        //              : "r"(get_current_task()->cr3));
        // asm volatile("mov %0, %%cr3"
        //              :
        //              : "r"(vmem_get_kernel_context()));
        uint32_t cr0;
        uint32_t *newPageDir = get_current_task()->context->pagedir;

        // serial_printf("Task context: 0x%x\n", newPageDir);
        // serial_printf("Kernel context: 0x%x\n", vmem_get_kernel_context()->pagedir);
        // vmem_activate_context(get_current_task()->context);
        // asm volatile("mov %0, %%cr3"
        //              :
        //              : "r"(newPageDir));
        // asm volatile("mov %%cr0, %0"
        //              : "=r"(cr0));
        // cr0 |= (1 << 31);
        // asm volatile("mov %0, %%cr0"
        //              :
        //              : "r"(cr0));
    }
    else
    {
    }
    outb(0x20, 0x20); // EOI
    return new_cpu;
}

void irq1_handler(void);

void irq2_handler(void)
{
    serial_println("IRQ2");
    outb(0x20, 0x20); // EOI
}

void irq3_handler(void)
{
    serial_println("IRQ3");
    outb(0x20, 0x20); // EOI
}

void irq4_handler(void)
{
    serial_println("IRQ4");
    outb(0x20, 0x20); // EOI
}

void irq5_handler(void)
{
    serial_println("IRQ5");
    outb(0x20, 0x20); // EOI
}

void irq6_handler(void)
{
    serial_println("IRQ6");
    outb(0x20, 0x20); // EOI
}

void irq7_handler(void)
{
    serial_println("IRQ7");
    outb(0x20, 0x20); // EOI
}

void irq8_handler(void)
{
    serial_println("IRQ8");
    outb(0xA0, 0x20);
    outb(0x20, 0x20); // EOI
}

void irq9_handler(void)
{
    serial_println("IRQ9");
    outb(0xA0, 0x20);
    outb(0x20, 0x20); // EOI
}

void irq10_handler(void)
{
    serial_println("IRQ10");
    outb(0xA0, 0x20);
    outb(0x20, 0x20); // EOI
}

void irq11_handler(void)
{
    serial_println("IRQ11");
    outb(0xA0, 0x20);
    outb(0x20, 0x20); // EOI
}

void irq12_handler(void)
{
    serial_println("IRQ12");
    outb(0xA0, 0x20);
    outb(0x20, 0x20); // EOI
}

void irq13_handler(void)
{
    serial_println("IRQ13");
    outb(0xA0, 0x20);
    outb(0x20, 0x20); // EOI
}

void irq14_handler(void)
{
    serial_println("IRQ14");
    outb(0xA0, 0x20);
    outb(0x20, 0x20); // EOI
}

void irq15_handler(void)
{
    serial_println("IRQ15");
    outb(0xA0, 0x20);
    outb(0x20, 0x20); // EOI
}
void print_cpu_state(struct cpu_state *cpu)
{
    serial_printf("eax: 0x%x\n", cpu->eax);
    serial_printf("ebx: 0x%x\n", cpu->ebx);
    serial_printf("ecx: 0x%x\n", cpu->ecx);
    serial_printf("edx: 0x%x\n", cpu->edx);
    serial_printf("esi: 0x%x\n", cpu->esi);
    serial_printf("edi: 0x%x\n", cpu->edi);
    serial_printf("ebp: 0x%x\n", cpu->ebp);
    serial_printf("intr: 0x%x\n", cpu->intr);
    serial_printf("error: 0b%b\n", cpu->error);
    serial_printf("eip: 0x%x\n", cpu->eip);
    serial_printf("cs: 0x%x\n", cpu->cs);
    serial_printf("eflags: 0x%x\n", cpu->eflags);
    serial_printf("esp: 0x%x\n", cpu->esp);
    serial_printf("ss: 0x%x\n", cpu->ss);
}

struct cpu_state *generic_irq_handler(struct cpu_state *cpu)
{
    serial_printf("General protection fault\n");
    // print_cpu_state(cpu);
    kill_current_task();
    while (1)
    {
    }
    return cpu;
}

struct cpu_state *pf_irq_handler(struct cpu_state *cpu)
{
    if (cpu->intr == 0xffffff80 || cpu->intr == 0xffffffff || cpu->intr == 0xf000ff63)
    {
        serial_printf("Page fault, killing process\n");
        kill_current_task();
        return cpu;
    }
    // if (cpu->intr == 4294967295)
    // {
    //     return cpu;
    // }
    const uint32_t ERR_PRESENT = 1 << 0;
    const uint32_t ERR_RW = 1 << 1;
    const uint32_t ERR_USER = 1 << 2;
    const uint32_t ERR_RESERVED = 1 << 3;
    const uint32_t ERR_INST = 1 << 4;

    serial_printf("Page fault\n");
    serial_printf("Tried to access 0x%x\n", cpu->intr);

    uint32_t present = cpu->error & ERR_PRESENT;
    uint32_t rw = cpu->error & ERR_RW;
    uint32_t user = cpu->error & ERR_USER;
    uint32_t reserved = cpu->error & ERR_RESERVED;
    uint32_t inst_fetch = cpu->error & ERR_INST;

    serial_printf("Possible causes: [ ");
    if (!present)
        serial_printf("Page not present ");
    if (rw)
        serial_printf("Page is read only ");
    if (user)
        serial_printf("Page is read only for user ");
    if (reserved)
        serial_printf("Overwrote reserved bits ");
    if (inst_fetch)
        serial_printf("Instruction fetch ");
    serial_printf("]\n");

    while (1)
    {
    }
    return cpu;
}
struct cpu_state *irq80_handler(struct cpu_state *cpu)
{
    switch (cpu->eax)
    {
    case 0:
        kputc(cpu->ebx);
        break;
    case 1:
        kill_current_task();
        break;
    default:
        serial_printf("Unknown syscall: %d\n", cpu->eax);
        break;
    }
    return cpu;
}