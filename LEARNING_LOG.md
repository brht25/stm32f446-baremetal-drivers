# Learning log

## 2026-10-05: Tool setup and Renode board file

### Done

- Installed Renode 1.17.0 and PulseView 0.4.2
- Wrote renode/nucleo_f446re.repl and loaded it in Renode

### What broke

- Renode.app blocked by macOS: The message said basically that Apple could not verify whether Renode.app is free of malware or not, so I had to go to the privacy and security section of system settings to open it anyway.
- PulseView nightly crashed: it was caused by a "dyld: Library not loaded" error that expects Python 3.12 from the Intel Homebrew installation. So I switched to 0.4.2.
- Renode says, "Couldn't start UI": the macOS build runs in console mode, and I will use the monitor prompt, socket terminals, and GDB instead of windows.

### Checked in the docs

- UM1724 : LD2 → PA5
- UM1724 : B1 → PC13
- UM1724 : USART2 (PA2/PA3) → ST-LINK virtual COM port
- RM0390 memory map: GPIOA at 0x4002 0000 (matches Renode)
- RM0390: SRAM1 112 KB + SRAM2 16 KB = 128 KB

### Learned

- .repl file: It is used to make adjustments, add devices, and change existing ones in your generic chip in Renode. The generic chip came from the first line with the "using" keyword. We created a UserLED peripheral at pin 5 of gpioPortA, then connected the output of pin 5 to that input of the UserLED we created. After that we created a UserButton at pin 13 of gpioPortC, then connected its output to the input of the same pin 13 of PortC. At last we adjusted the sram and flash values to be the same as the real F446RE, so Renode will warn if the firmware uses more memory than the real chip has.



## 2026-10-06: 
# Logic analyzer test

- Analyzer detected in PulseView as "Saleae Logic" with the fx2lafw driver.
- Pin labels CH1–CH8 map to PulseView D0–D7 (CH1 = D0).
- Grounding each channel shows a flat low; all 8 channels work.
- Unconnected inputs read high on this analyzer, but floating inputs are undefined.


### HAL blink in Renode (toolchain check)


#### What broke
- Build error "LD2_GPIO_Port undeclared": BSP uses LED2_GPIO_PORT / LED2_PIN; compiler hint.
- Renode: "File does not exist" for blink.resc: file never saved there; created it from Terminal.
- LED never changed in Renode: PC jumping between 2 addresses = empty loop · MODER said PA5 output · ODR = 0 → code never ran · cause: lines after while (1), the USER CODE END 3 trap.

#### Checked in the docs
- RM0390 GPIO MODER: 2 bit per pin. 01 for output mode and 10 for alternate funciton mode.
- RM0390 SYSCFG_EXTICR4: bits 4-7 are belong to EXTI13. on decoding 0x20 those bits are 0010. so it is the PC. So value 2 gives us PC13.

#### Learned

- MODER: 2 bits per pin, bits 2n+1:2n, values 00/01/10/11; decoded 0xA80004A0
- Multi-bit fields hold a number: EXTICR4 bits 7:4 = 2 → port C
- Linker script MEMORY block → the RAM/FLASH report; must match the .repl; initial SP = top of RAM
- Renode commands: cpu PC, sysbus FindSymbolAt, ReadDoubleWord, logLevel -1

## Renode vs. real hardware

- Generic STM32F4 has 2 MB flash / 256 KB RAM; F446RE has 512 KB / 128 KB.
  Overridden in the .repl. Out-of-range access in Renode logs a warning,
  while real silicon would fault.
- No GUI on macOS build; using console mode.
- Flash ACR cache/prefetch bits ignored, harmless.
- SYSCFG EXTICR4 not implemented, any value accepted. a wrong value could pass in Renode but fail on the board. This needs checking in Week 3.

## 2026-10-12: RCC and GPIO registers for the PA5 blink

### Checked in the docs
- RM0390 memory map: RCC base address is 0x4002 3800, GPIOA base address is 0x4002 0000, GPIOA is on the AHB1 bus.
- RM0390 RCC_AHB1ENR: AHB1ENR 's offset is 0x30. and its full address is 0x4002 3830. reset value is 0x0000 0000. GPIOA is on bit 0. 
- RM0390 RCC_AHB1RSTR: It is the reset register. so it would reset the GPIOA if I would set bit 0 to high.
- RM0390 GPIOx_ODR: offset is 0x14. full address is 0x4002 0014. bits 31:16 is reserved.Bits 15:0 = one per bit.
- RM0390 GPIOx_BSRR: its offset is 0x18. full adress is 0x4002 0018. writing 0 does nothing and any read returns 0. 15:0 are set bits. 31:16 are reset bits. 
- UM1724 LD2: It is on when pa5 is high, and off when pa5 is low. 

### What broke
- I almost mix up ENR and RSTR because they sit next tı each other in rm0390 with same bit layout. If I would set bit 0 to high in RSTR instead of ENR, it would stay in reset until I set the bit to low again. So LED would not work until I set the bit to low again. and there would be no error. 

### Learned
- Rule for picking the right RCC enable register: We should find the parent bus for peripheral then match the bus name to the RCC register.
- read-modify-write: It is a three-step sequence where a processor reads an existing register value into CPU register, alters specific bits using logical operations without disturbing the others, and writes the updated value back to that same address.
- why 1U and not 1: a plain 1 is a signed int. 1 << 31 shifts into the sign bit, which is undefined behaviour in C. 1U is unsigned, so every shift up to 31 is well defined.
-  clear-then-set: clear using bitwise oparetion "and" inverted bitwise. then set it using bitwise operation or.
- using "=" instead of "|=" on RCC_AHB1ENR would clear the bits we didnt want to change. No error would appear but enabling a GPIOX would disable everything else. 
- BSRR = vs ODR ^=: BSRR= is a atomic operation. It is a single write, so nothing can happen  in beetween. For ODR ^= , if an interrupt changes another GPIOA pin in between, the write undoes its change. 
- busy wait: It is needed to waste time on purpose, so we can see the loop is working. Otherwise, we couldnt see it properly since the LED changes too fast to see(cpu speed is about 16 MHz).
- superloop: Main never return to os. because there is no os in the chip. so we need to keep everything on a loop. If main does return, it goes back to startup code, and what happens next depends on that file. 

### Plan for the PA5 blink
- Setup:
1. RCC_AHB1ENR:  set bit 0 with |=
2. GPIOA_MODER:  clear bits 11:10 with &= ~mask, then set them to 01 with |=
- Superloop:
3. GPIOA_BSRR = bit 5     → LED on
4. busy wait
5. GPIOA_BSRR = bit 21     → LED off
6. busy wait

