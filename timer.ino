
#include <avr/wdt.h>            // library for default watchdog functions
#include <avr/interrupt.h>      // library for interrupts handling
#include <avr/sleep.h>          // library for sleep
#include <avr/power.h>          // library for power control

#include <U8x8lib.h>


class EventQueue
{
    public:
        enum EventType
        {
            NoEvent,
            Refresh,
            Button,
            Left, 
            Right
        };

        EventQueue()
            : m_head(0), m_tail(0)
        {
            
        }

        void push(EventType type)
        {

            m_queue[m_head] = type;
            if(++m_head == size) m_head = 0;
        }

        EventType next()
        {
            if(m_head == m_tail) return NoEvent;


            EventType type = m_queue[m_tail];
            
            if(++m_tail == size) m_tail = 0;

            return type; 
        }

    private: 
        static constexpr uint8_t size = 64;
    
        uint8_t m_queue[size];

        // If m_head is not volatile, the compiler optimize the condition 
        // if(m_head == m_tail) to if(true)
        volatile uint8_t m_head;
        volatile uint8_t m_tail;
    
};

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
        static constexpr uint32_t calibDuration = 2000000; // us = 2s
        
        uint32_t m_calibSum;
        uint32_t m_calibTotal;
        uint32_t m_calibPrevTimestamp;
        uint32_t m_calib;
        bool     m_calibDone;

        uint32_t m_periodUpdate;
        uint32_t m_periodCpt;

        EventQueue & m_events;
};


class RotaryEncoder
{
    public:

        enum RotaryState
        {
            RotaryStart,
            RotaryOffOnRight,
            RotaryOffOffRight,
            RotaryOnOffRight,
        
            RotaryOnOffLeft,
            RotaryOffOffLeft,
            RotaryOffOnLeft,
        };

        RotaryEncoder(EventQueue & events)
            : m_rotaryState(RotaryStart), m_events(events)
        {
        
            init();
        }

        void init()
        {
            pinMode(2, INPUT_PULLUP);
            pinMode(4, INPUT_PULLUP);
            pinMode(7, INPUT_PULLUP);
    
            // Enable Change interrupts for pins 23...16.
            PCICR = bit(PCIE2);
            // Set mask to enable Change interrupt on pins 18, 20 and 23.
            PCMSK2 = bit(PCINT18) | bit(PCINT20) | bit(PCINT23);

        }

        void update()
        {
            if(m_states[0] != digitalRead(2))
            {
                m_states[0] = !m_states[0];
        
                if( m_states[0] && m_rotaryState == RotaryOffOffRight) 
                    m_rotaryState = RotaryOnOffRight;
                else if(!m_states[0] && m_rotaryState == RotaryOnOffLeft) 
                    m_rotaryState = RotaryOffOffLeft;
                else if( m_states[0] && m_rotaryState == RotaryOffOnLeft)
                {
                    m_rotaryState = RotaryStart;

                    m_events.push(EventQueue::Left);
                }
                else 
                    m_rotaryState = RotaryStart;
        
                if(!m_states[0] && m_rotaryState == RotaryStart) 
                    m_rotaryState = RotaryOffOnRight;
            }
            else if(m_states[2] != digitalRead(7))
            {
                m_states[2] = !m_states[2];
        
                if( m_states[2] && m_rotaryState == RotaryOffOffLeft) 
                    m_rotaryState = RotaryOffOnLeft;
                else if(!m_states[2] && m_rotaryState == RotaryOffOnRight) 
                    m_rotaryState = RotaryOffOffRight;
                else if( m_states[2] && m_rotaryState == RotaryOnOffRight)
                {
                    m_rotaryState = RotaryStart;

                    m_events.push(EventQueue::Right);
                }
                else 
                    m_rotaryState = RotaryStart;
        
                if(!m_states[2] && m_rotaryState == RotaryStart) m_rotaryState = 4;
            }
            else if(m_states[1] != digitalRead(4))
            {
                m_states[1] = !m_states[1];

                if(!m_states[1]) 
                    m_events.push(EventQueue::Button);                
            }
        }

    private: 
        
        bool          m_states[3];
        RotaryState   m_rotaryState;
        EventQueue  & m_events;  
};

EventQueue     events;
WatchdogUpdate watchdog(WatchdogUpdate::period125ms, 1000, events);
RotaryEncoder  encoder(events);


U8X8_UC1604_JLX19264_4W_HW_SPI u8x8(/* cs=*/ 16, /* dc=*/ 14, /* reset=*/ 15); 

ISR(WDT_vect)
{
    watchdog.update();
}


ISR(PCINT2_vect) 
{
    encoder.update();
}


void setup()
{

    Serial.begin(115200);

   
    u8x8.begin();
    u8x8.setFlipMode(true);

}


void loop()
{
    int cpt = 0;
    int nbRefresh = 0;

    while(true)
    {
        // bool isExternalEvent = false;

        EventQueue::EventType event = events.next();

        if(event != EventQueue::NoEvent)
        {
            
            if(event != EventQueue::Refresh)
            {
                ++cpt;
                ++nbRefresh;
            }
            else 
                nbRefresh = 0;
                
            u8x8.setFont(u8x8_font_chroma48medium8_r);
            
            u8x8.drawString(0,0,"Hello World!");

            u8x8.setCursor(15, 0);
            u8x8.print(cpt);
        }

        
        if(!watchdog.isCalibrating() && nbRefresh >= 3)
        {
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_mode();
            //nbRefresh = 0;
        }
    }

}
