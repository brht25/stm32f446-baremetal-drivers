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
- RM0390 GPIO MODER: <how many bits per pin, what 01 / 10 mean> 2 bit per pin. 01 for output mode and 10 for alternate funciton mode.
- RM0390 SYSCFG_EXTICR4: <which bits belong to EXTI13, what value 2 means> bits 4-7 are belong to EXTI13. on decoding 0x20 those bits are 0010. so it is the PC. So value 2 gives us PC13.

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