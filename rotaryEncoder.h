
#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include "eventQueue.h"

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

#endif
