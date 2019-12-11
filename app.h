
#ifndef APP_H_
#define APP_H_

#include <U8x8lib.h>

#include "watchdogUpdate.h"
#include "rotaryEncoder.h"
#include "screenLight.h"
#include "sprites.h"

class App
{
    public:
        static constexpr uint8_t hourDigit   = 0;
        static constexpr uint8_t minuteDigit = 1;
        static constexpr uint8_t secondDigit = 2;

        static constexpr uint8_t lcdCS    = 16;
        static constexpr uint8_t lcdDC    = 14;
        static constexpr uint8_t lcdReset = 15;
        
        using U8x8 = U8X8_UC1604_JLX19264_4W_HW_SPI;

        App()
            : m_watchdog(WatchdogUpdate::period500ms, 1000, m_events),
              m_encoder(m_events),
              m_nbRefreshWithoutEvents(0),
              m_u8x8(lcdCS, lcdDC, lcdReset),
              m_timer(0), m_current(2)
        {

        }

        void init()
        {
            m_u8x8.begin();
            m_u8x8.setFlipMode(true);

            displayTimer();
            displayButton();
        }

        void drawSprite(uint8_t x, uint8_t y, uint8_t size, const uint8_t * sprite, bool inverse = false)
        {
            static uint8_t tmp[8];

            for(uint8_t i = 0; i < size; ++i)
            {
                if(inverse)
                {
                    for(uint8_t j = 0; j < 8; ++j)
                        tmp[j] = sprite[i * 8 + j] ^ 255;

                    m_u8x8.drawTile(x + i, y, 1, tmp);
                }
                else
                    m_u8x8.drawTile(x + i, y, 1, sprite + i * 8);
            }
        }

        void displayDigit(uint8_t digit)
        {

            uint8_t val = hourDigit   == 0 ?  m_timer / 3600 : 
                          minuteDigit == 1 ? (m_timer / 60) % 60  : 
                          secondDigit == 2 ?  m_timer % 60 : 0;

            for(uint8_t i = 0; i < 3; ++i)
            {
                drawSprite(9 * digit + 0, i, 3, Sprite::digits[val / 10][i], m_current == digit);
                drawSprite(9 * digit + 3, i, 3, Sprite::digits[val % 10][i], m_current == digit);
            }
        }

        void displayButton()
        {
            drawSprite(7, 5, 10, Sprite::start[0], m_current == 3);
            drawSprite(7, 6, 10, Sprite::start[1], m_current == 3);
        }

        void displayTimer()
        {

            displayDigit(0);
            displayDigit(1);
            displayDigit(2);

            for(uint8_t i = 0; i < 3; ++i)
            {
                drawSprite(6, i, 3, Sprite::dot[i]);
                drawSprite(15, i, 3, Sprite::dot[i]);
            }   
        }

        void update(EventQueue::EventType event)
        {
            if(event == EventQueue::Refresh) 
            {
                ++m_nbRefreshWithoutEvents;

                if(m_timer > 0)
                {
                    --m_timer;
                    displayTimer();
                }
            }
            else 
                m_nbRefreshWithoutEvents = 0;

            if(event == EventQueue::Right || event == EventQueue::Left)
            {
               
                if(event == EventQueue::Right)
                {
                    if(m_current + 1 == 4) m_current = 0;
                    else ++m_current;
                }
                else 
                {
                    if(m_current == 0) m_current = 3;
                    else --m_current;
                }

                displayTimer();
                displayButton();
            }
        }

        bool update()
        {
            
            if(m_watchdog.isCalibrating())
            {
                m_events.empty();
                return false;
            }

            EventQueue::EventType event = m_events.next();

            while(event != EventQueue::NoEvent)
            {      
                update(event);  
                event = m_events.next();
            }

            if(m_nbRefreshWithoutEvents > 10)
                m_screenLight.stop();
            else 
                m_screenLight.start();

            return m_nbRefreshWithoutEvents > 2;
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
        EventQueue     m_events;
        WatchdogUpdate m_watchdog;
        RotaryEncoder  m_encoder;
        ScreenLight    m_screenLight;
        uint32_t       m_nbRefreshWithoutEvents;

        U8x8           m_u8x8;
        uint32_t       m_timer;
        uint32_t       m_current;
};

#endif
