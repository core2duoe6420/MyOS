OUTPUT_FORMAT(binary)
ENTRY(START)
SECTIONS
{
	.text 0x20100000 : { *(.text) }
	.data : { *(.data) }
	.rodata : { *(.rodata) *(.rodata*) }
	.bss : { *(.bss) }
	/DISCARD/ : { *(.note.GNU-stack) *(.comment) }
} 
