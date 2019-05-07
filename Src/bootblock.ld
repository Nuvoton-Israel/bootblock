
SECTIONS
{
    .START 0xFFFD6000:
    {
        KEEP(./Src/bootblock_start.o*(.text .text.*))
    }

    .VERSION 0xFFFD6040:
    {
        KEEP(./Src/bootblock_version.o*(.rodata .rodata.*))
    }

    .BOOTBLOCK 0xFFFD6100:
    {
        __bootblock_start = .;
        *(.text*)
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
        *(.data .data.*)
        . = ALIGN(4);
        *(COMMON)
        . = ALIGN(4);
        *(.bss*)
        __bootblock_end = .;
    }

    .init :
    {
        KEEP (*(SORT_NONE(.init)))
    }

    .ARM.exidx :
    {
        __exidx_start = .;
	*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	__exidx_end = .;
    }

    .heap (NOLOAD):
    {
        . = ALIGN(64);
        PROVIDE(end = .);
    }

    .rel.plt        : { *(.rel.plt) }
    .interp         : { *(.interp) }
}