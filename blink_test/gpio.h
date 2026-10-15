#ifndef GPIO_H
#define GPIO_H
#include <stdint.h>
#include <stddef.h>

typedef struct {

    uint32_t MODER;   //0x00
    uint32_t OTYPER;   //0x04
    uint32_t OSPEEDER;   //0x08
    uint32_t PUPDR;   //0x0c
    uint32_t IDR;   //0x10
    uint32_t ODR;   //0x14
    uint32_t BSRR;  //0x18
    uint32_t LCKR;  //0x1c
    uint32_t AFRL;  //0x20
    uint32_t AFRH;  //0x24

} gpio_regs;

_Static_assert(offsetof(gpio_regs, MODER) == 0x00, "offset of MODER is wrong");
_Static_assert(offsetof(gpio_regs, OTYPER) == 0x04, "offset of OTYPER is wrong");
_Static_assert(offsetof(gpio_regs, OSPEEDER) == 0x08, "offset of OSPEEDER is wrong");
_Static_assert(offsetof(gpio_regs, PUPDR) == 0x0c, "offset of PUPDR is wrong");
_Static_assert(offsetof(gpio_regs, IDR) == 0x10, "offset of IDR is wrong");
_Static_assert(offsetof(gpio_regs, ODR) == 0x14, "offset of ODR is wrong");
_Static_assert(offsetof(gpio_regs, BSRR) == 0x18, "offset of BSRR is wrong");
_Static_assert(offsetof(gpio_regs, LCKR) == 0x1c, "offset of LCKR is wrong");
_Static_assert(offsetof(gpio_regs, AFRL) == 0x20, "offset of AFRL is wrong");
_Static_assert(offsetof(gpio_regs, AFRH) == 0x24, "offset of AFRH is wrong");



#endif 