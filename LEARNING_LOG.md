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


## Renode vs. real hardware

- Generic STM32F4 has 2 MB flash / 256 KB RAM; F446RE has 512 KB / 128 KB.
  Overridden in the .repl. Out-of-range access in Renode logs a warning,
  while real silicon would fault.
- No GUI on macOS build; using console mode.
