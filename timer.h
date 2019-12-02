
#ifndef TIMER_H_
#define TIMER_H_

#include <string.h>
#include "color.h"

class Timer
{
    public:
        Timer()
            : m_duration(0),
              m_color(White),
              m_name("")
        {

        }

        Timer & operator = (const Timer & other)
        {
            m_duration = other.m_duration;
            m_color = other.m_color;
            memcpy(m_name, other.m_name, 32);
        }

        void decrement()
        {
            if(m_duration > 0) 
                --m_duration;
        }

        bool isFinished() const
        {
            return m_duration == 0;
        }

        uint32_t getDuration() const
        {
            return m_duration;
        }

        const int8_t * getName() const
        {
            return m_name;
        }

        const Color getColor() const
        {
            return m_color;
        }

    private: 
        uint32_t m_duration;
        Color    m_color;
        int8_t   m_name[32];
};

#endif 
