
#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_

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

        void empty()
        {
            m_tail = m_head;
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
        uint8_t m_tail;
    
};

#endif
