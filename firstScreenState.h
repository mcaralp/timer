
#ifndef FIRST_SCREEN_STATE_H_
#define FIRST_SCREEN_STATE_H_

#include "state.h"
#include "appState.h"
#include "eventQueue.h"

class FirstScreenState
{
    public:
        FirstScreenState()
        {
        }

        void init(State & state)
        {
            m_cpt = 0;
        }

        AppState update(State & state, EventQueue::EventType event)
        {
            if(event != EventQueue::Refresh)
            {
                ++m_cpt;
            }
                
            state.u8x8.setFont(u8x8_font_chroma48medium8_r);
            
            state.u8x8.drawString(0,0,"Hello World!");

            state.u8x8.setCursor(15, 0);
            state.u8x8.print(m_cpt);

            return FirstScreen;
        }

    private:
        uint32_t m_cpt;
};

#endif
