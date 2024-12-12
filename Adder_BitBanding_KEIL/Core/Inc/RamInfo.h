#ifndef RamInfo_H_
#define RamInfo_H_

// Arm Compiler : Version 6
#define AT_20  __attribute__((section(".ARM.__at_0x20000000")))
#define AT_22  __attribute__((section(".ARM.__at_0x22000000")))

// Arm Compiler : Version 5
// Error: L6971E: main.o(.data) type RW incompatible with main.o(.ARM.__AT_0x20000000) type ZI in er RW_IRAM1.
//#define AT_20  __attribute__((at(0x20000000)))
//#define AT_22  __attribute__((at(0x22000000)))

#endif
