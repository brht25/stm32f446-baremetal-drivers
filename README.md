
# STM32F446RE bare-metal drivers

Register-level drivers (GPIO, interrupts, UART, timer, I2C) for the
Nucleo-F446RE, written without HAL, using only the CMSIS device header.

## Status

Week 1: toolchain and Renode emulation setup.

## Roadmap

- [ ] Register-level LED blink, own startup file and linker script
- [ ] GPIO driver, SysTick, button interrupt
- [ ] UART driver with interrupt RX and a small command line
- [ ] I2C driver and BME280 sensor
- [ ] Timer-driven sampling, v1.0
