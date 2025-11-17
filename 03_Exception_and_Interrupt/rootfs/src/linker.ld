ENTRY(_start)

SECTIONS
{
  . = 0x40000000;
  .text : {
    *(.text._start)
    *(.text)
  }

  .data : { *(.data) }

  .bss : {
    __bss_start = .;
    *(.bss)
    __bss_end = .;
  }
}