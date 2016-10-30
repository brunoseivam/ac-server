#include "ir.h"
#include "os_type.h"
#include "pwm.h"
#include "mem.h"
#include "osapi.h"
#include "xtensa/hal.h"

/*
 * IR Pin: Oak's P5 = ESP's GPIO4
 */

#define IR_IO_MUX   PERIPHS_IO_MUX_GPIO4_U
#define IR_IO_NUM   4
#define IR_IO_FUNC  FUNC_GPIO4
#define IR_IO_MASK  (1 << IR_IO_NUM)

#define IR_PWM_CHANNELS 1

// NEC Protocol
#define NEC_FREQ    38000           // Hz
#define NEC_PERIOD  (1000000/38000) // us

// Length of pulse trains, in # of pulses
#define HDR_MARK    (9000 / NEC_PERIOD)
#define HDR_SPACE   (4500 / NEC_PERIOD)
#define BIT_MARK    (560  / NEC_PERIOD)
#define ONE_SPACE   (1690 / NEC_PERIOD)
#define ZERO_SPACE  (560  / NEC_PERIOD)

// Header: mark+space(2), each bit: mark+space(2*32bits=64), trail:mark+space(2)
#define NTRAINS     (2+64+2)

static uint16_t address = 0x10AF;

// PWM doesn't work, so let's lower the level
static void ICACHE_FLASH_ATTR ir_do_send(uint32_t data[NTRAINS])
{
    // Disable interrupts
    unsigned intmask = xthal_get_intenable();
    xthal_set_intenable(0);

    unsigned int i;
    for(i = 0; i < 2 + 64 + 2; ++i)
    {
        bool mark = !(i & 1);
        if(mark) // Mark
        {
            uint32_t pulses = data[i];
            while(pulses--)
            {
                ir_set(true);
                os_delay_us(NEC_PERIOD/2);
                ir_set(false);
                os_delay_us(NEC_PERIOD/2);
            }
        }
        else // Space
        {
            os_delay_us(NEC_PERIOD*data[i]);
        }

    }

    xthal_set_intenable(intmask);
}

void ir_set_address_csum(uint8_t addr, uint8_t csum)
{
    address = ((uint16_t)addr << 8) | ((uint16_t)csum);
}

void ir_set_address(uint8_t addr)
{
    ir_set_address_csum(addr, ~addr);
}

void ir_init (void)
{
    gpio_output_set(0, 0, IR_IO_MASK, 0);
    os_printf("IR initialized\n");
}

void ir_set(bool on)
{
    if(on)
        gpio_output_set(IR_IO_MASK, 0, 0, 0);
    else
        gpio_output_set(0, IR_IO_MASK, 0, 0);
}

// Build pulse trains list
void ir_send_csum (uint8_t command, uint8_t csum)
{
    uint32_t data = ((uint32_t)address << 16) | ((uint32_t)command << 8) | (uint32_t)csum;
    os_printf("Sending %08X\n", data);

    unsigned int trains[2+64+2];

    unsigned int *datap = trains;
    *datap++ = HDR_MARK;
    *datap++ = HDR_SPACE;

    uint32_t mask;
    for(mask = 1UL << 31; mask; mask >>= 1)
    {
        *datap++ = BIT_MARK;
        *datap++ = data & mask ? ONE_SPACE : ZERO_SPACE;
    }

    *datap++ = BIT_MARK;
    *datap++ = 0;

    ir_do_send(trains);
}

void ir_send(uint8_t command)
{
    ir_send_csum(command, ~command);
}
