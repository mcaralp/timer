
#ifndef LEDS_H_
#define LEDS_H_

struct Leds
{
    uint32_t period;
    uint32_t size;
    uint8_t  data[512][3];

};

#endif