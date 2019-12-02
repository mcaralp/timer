
#ifndef STATE_H_
#define STATE_H_ 

#include <U8x8lib.h>
#include "eventQueue.h"

struct State
{
    static constexpr uint8_t lcdCS    = 16;
    static constexpr uint8_t lcdDC    = 14;
    static constexpr uint8_t lcdReset = 15;
    using U8x8 = U8X8_UC1604_JLX19264_4W_HW_SPI;

    State()
        : u8x8(lcdCS, lcdDC, lcdReset)
    {

    }

    U8x8 u8x8; 

};

#endif
