/* STM32F767xx minimal GNU ld script */
ENTRY(Reset_Handler)

/* Define memory regions (from STM32F767 datasheet) */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K  /* 2 MB Flash */
  DTCMRAM (rwx): ORIGIN = 0x20000000, LENGTH = 128K  /* 128 KB TCM RAM */
  RAM   (rwx) : ORIGIN = 0x20020000, LENGTH = 368K   /* SRAM1+SRAM2 = 368 KB */
}

/* Top of stack in DTCM RAM */
_estack = ORIGIN(DTCMRAM) + LENGTH(DTCMRAM);

SECTIONS
{
  /* Vector table at start of flash */
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
    KEEP(*(.init))
    KEEP(*(.fini))
  } > FLASH

  /* C++ unwind tables */
  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
  .ARM.exidx : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } > FLASH

  /* Constructors / destructors */
  .preinit_array :
  {
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
  } > FLASH

  .init_array :
  {
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN(__init_array_end = .);
  } > FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*)))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN(__fini_array_end = .);
  } > FLASH

  /* Initialized data in RAM, load from Flash */
  .data : AT (ADDR(.fini_array) + SIZEOF(.fini_array))
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM
  _sidata = LOADADDR(.data);

  /* Zero-initialized data */
  .bss (NOLOAD) :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

  /* Heap and stack (grow toward each other) */
  __heap_start = .;
  __heap_end   = ORIGIN(RAM) + LENGTH(RAM);

  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);
  } >RAM

}
