
#ifndef TIMER_LIST_H_
#define TIMER_LIST_H_

#include "timer.h"

template<uint8_t myMaxSize>
class TimerList
{
    public:
        static constexpr uint8_t maxSize = myMaxSize;

        TimerList()
            : m_size(0)
        {

        }

        const Timer & operator[] (uint32_t index) const
        {
            return m_timers[index];
        }

        Timer & operator[] (uint32_t index)
        {
            return m_timers[index];
        }

        uint8_t size() const
        {
            return m_size;
        }

        void push(const Timer & timer)
        {
            m_timers[m_size++] = timer;
        }

        void remove(uint32_t index)
        {
            for(uint32_t i(index); i + 1 < m_size; ++i)
                m_timers[i] = m_timers[i + 1];

            --m_size;
        }

    private:

        Timer    m_timers[maxSize];
        uint32_t m_size;
};

#endif 
