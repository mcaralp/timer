
#ifndef WATCHDOG_UPDATE_H_
#define WATCHDOG_UPDATE_H_

#include "eventQueue.h"
#include <avr/wdt.h>

class WatchdogUpdate
{
    public:  
        enum Period
        {
            period16Ms  = 0,
            period32Ms  = 1,
            period64ms  = 2,
            period125ms = 3,
            period250ms = 4,
            period500ms = 5,
            period1s    = 6,
            period2s    = 7,
            period4s    = 8,
            period8s    = 9
        };

        WatchdogUpdate(Period period, uint32_t periodUpdate, EventQueue & events)
            : m_calibSum(0), m_calibTotal(0), 
              m_calibPrevTimestamp(0), m_calibDone(false), 
              m_calib(0), m_periodUpdate(periodUpdate * 1000),
              m_events(events)
        {
            setPeriod(period);
        }

        void setPeriod(Period period)
        {
            static constexpr uint8_t periods[] = {
                0b00000000, // 16 ms
                0b00000001, // 32 ms
                0b00000010, // 64 ms
                0b00000011, // 125ms
                0b00000100, // 250 ms
                0b00000101, // 500 ms
                0b00000110, // 1 s 
                0b00000111, // 2 s
                0b00100000, // 4 s
                0b00100001  // 8 s
            };

            WDTCSR |= bit(WDCE) | bit(WDE);
            WDTCSR =  bit(WDIE) | periods[period]; 

            m_calibSum = 0;
            m_calibTotal = 0;
            m_calibPrevTimestamp = 0;
            m_calibDone = false;
        }

        void update()
        {
            if(m_calibDone)
            {
                m_periodCpt += m_calib;
                if(m_periodCpt >= m_periodUpdate)
                {
                    m_events.push(EventQueue::Refresh);
                    m_periodCpt -= m_periodUpdate;
                }
            }
            else
            {
                uint32_t now = micros();
                
                if(m_calibPrevTimestamp != 0)
                {
                    m_calibSum += now - m_calibPrevTimestamp;
                    m_calibTotal += 1;
                }

                if(m_calibSum >= calibDuration)
                {
                     m_calib = m_calibSum / m_calibTotal;
                     m_calibDone = true;
                }

                m_calibPrevTimestamp = now;
            }
        }

        bool isCalibrating() const
        {
            return !m_calibDone;
        }

    private:
        static constexpr uint32_t calibDuration = 2000000; // 2s
        
        uint32_t m_calibSum;
        uint32_t m_calibTotal;
        uint32_t m_calibPrevTimestamp;
        uint32_t m_calib;
        bool     m_calibDone;

        uint32_t m_periodUpdate;
        uint32_t m_periodCpt;

        EventQueue & m_events;
};

#endif
