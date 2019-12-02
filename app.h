
#ifndef APP_H_
#define APP_H_

#include "watchdogUpdate.h"
#include "rotaryEncoder.h"
#include "appState.h"
#include "state.h"
#include "firstScreenState.h"

class App
{
    public:

        App()
            : m_watchdog(WatchdogUpdate::period125ms, 1000, m_events),
              m_encoder(m_events),
              m_timerState(FirstScreen), m_nbRefreshWithoutEvents(0)
        {
            
            m_state.u8x8.begin();
            m_state.u8x8.setFlipMode(true);

            m_firstScreen.init(m_state);
        }

        bool update()
        {
            if(m_watchdog.isCalibrating()) return true;

            EventQueue::EventType event = m_events.next();

            while(event != EventQueue::NoEvent)
            {
                if(event == EventQueue::Refresh) 
                    ++m_nbRefreshWithoutEvents;
                else 
                    m_nbRefreshWithoutEvents = 0;

                switch(m_timerState)
                {
                    case FirstScreen:  
                        m_timerState = m_firstScreen.update(m_state, event);
                        break;
                }

                event = m_events.next();
            }

            return m_nbRefreshWithoutEvents > 3;
        }

        void updateWatchdog()
        {
            m_watchdog.update();
        }

        void updateEncoder()
        {
            m_encoder.update();
        }

    private:
        State          m_state;
        EventQueue     m_events;
        WatchdogUpdate m_watchdog;
        RotaryEncoder  m_encoder;
        AppState       m_timerState;
        
        // Screens

        FirstScreenState m_firstScreen;
        uint32_t         m_nbRefreshWithoutEvents;
};

#endif
