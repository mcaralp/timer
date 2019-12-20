
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

        enum State
        {
            SelectAButton,
            HoursButton,
            MinutesButton,
            SecondsButton,
            StartButton
        };

        enum BackgroundState
        {
            Sleep,
            Active,
            Animation
        };

        App()
            : m_watchdog(WatchdogUpdate::period500ms, 1000, m_events),
              m_encoder(m_events),
              m_nbRefreshWithoutEvents(0),
              m_u8x8(lcdCS, lcdDC, lcdReset),
              m_timer(0), m_current(2),
              m_state(SelectAButton),
              m_calibrated(false),
              m_animation(false),
              m_powerSave(false)
        {

        }

        void init()
        {
            m_u8x8.begin();
            m_u8x8.setFlipMode(true);

        }
        

        void drawSprite(uint8_t x, uint8_t y, uint8_t size, const uint8_t * sprite, bool inverse = false)
        {
            static uint8_t tmp[8];


            for(uint8_t i = 0; i < size; ++i)
            {
                for(uint8_t j = 0; j < 8; j++)
                {
                    if(inverse)
                        tmp[j] = u8x8_pgm_read(sprite + i * 8 + j) ^ 255;
                    else 
                        tmp[j] = u8x8_pgm_read(sprite + i * 8 + j);
                }
                
                m_u8x8.drawTile(x + i, y, 1, tmp);
            }
        }

        void displayDigit(uint8_t digit)
        {

            uint8_t val = hourDigit   == digit ?  m_timer / 3600 : 
                          minuteDigit == digit ? (m_timer / 60) % 60  : 
                          secondDigit == digit ?  m_timer % 60 : 0;

            for(uint8_t i = 0; i < 3; ++i)
            {
                drawSprite(9 * digit + 0, i + 1, 3, Sprite::digits[val / 10][i], m_current == digit);
                drawSprite(9 * digit + 3, i + 1, 3, Sprite::digits[val % 10][i], m_current == digit);
            }


            if(m_current == digit && m_state != SelectAButton)
            {
                for(uint8_t i = 0; i < 6; ++i)
                    drawSprite(9 * digit + i, 4, 1, Sprite::underline);
            }
        }

        void displayButton()
        {
            m_u8x8.clearLine(5);
            m_u8x8.clearLine(6);

            if(m_state != StartButton)
            {
                drawSprite(7, 5, 10, Sprite::start[0], m_current == 3);
                drawSprite(7, 6, 10, Sprite::start[1], m_current == 3);
            }
            else 
            {
                drawSprite(8, 5, 8, Sprite::stop[0], m_current == 3);
                drawSprite(8, 6, 8, Sprite::stop[1], m_current == 3);
            }
        }

        void displayTimer()
        {
            m_u8x8.clearLine(4);

            displayDigit(0);
            displayDigit(1);
            displayDigit(2);

            for(uint8_t i = 0; i < 3; ++i)
            {
                drawSprite(6, i + 1, 3, Sprite::dot[i]);
                drawSprite(15, i + 1, 3, Sprite::dot[i]);
            }   
        }

        void update(EventQueue::EventType event)
        {
            if(event == EventQueue::Refresh) 
                ++m_nbRefreshWithoutEvents;
            else 
                m_nbRefreshWithoutEvents = 0;

            switch(m_state)
            {
                case SelectAButton:
                    if(event == EventQueue::Right)
                    {
                        if(m_current + 1 == 4) m_current = 0;
                        else ++m_current;

                        m_animation = false;

                        displayTimer();
                        displayButton();
                    }
                    if(event == EventQueue::Left)    
                    {
                        if(m_current == 0) m_current = 3;
                        else --m_current;

                        m_animation = false;

                        displayTimer();
                        displayButton();
                    }
                    else if(event == EventQueue::Button)
                    {
                        m_animation = false;

                        if(m_current == hourDigit)
                        {
                            m_state = HoursButton;
                            displayTimer();
                        }
                        else if(m_current == minuteDigit)
                        {
                            m_state = MinutesButton;
                            displayTimer();
                        }
                        else if(m_current == secondDigit)
                        {
                            m_state = SecondsButton;
                            displayTimer();
                        }
                        else if(m_timer > 0)
                        {
                            m_state = StartButton;
                            displayButton();
                        }
                    }
                    break;

                case HoursButton:
                    if(event == EventQueue::Right)
                    {
                        m_timer += 3600;
                        displayTimer();
                    }
                    else if(event == EventQueue::Left)
                    {
                        if(m_timer > 3600) m_timer -= 3600;
                        else m_timer = 0;
                        displayTimer();
                    }
                    else if(event == EventQueue::Button)
                    {
                        m_state = SelectAButton;
                        displayTimer();
                    }
                    break;

                case MinutesButton:
                    if(event == EventQueue::Right)
                    {
                        m_timer += 60;
                        displayTimer();
                    }
                    else if(event == EventQueue::Left)
                    {
                        if(m_timer > 60) m_timer -= 60;
                        else m_timer = 0;
                        displayTimer();
                    }
                    else if(event == EventQueue::Button)
                    {
                        m_state = SelectAButton;
                        displayTimer();
                    }
                    break;

                case SecondsButton:
                    if(event == EventQueue::Right)
                    {
                        ++m_timer;
                        displayTimer();
                    }
                    else if(event == EventQueue::Left)
                    {
                        if(m_timer > 0) --m_timer;
                        displayTimer();
                    }
                    else if(event == EventQueue::Button)
                    {
                        m_state = SelectAButton;
                        displayTimer();
                    }
                    break;

                case StartButton:
                    if(event == EventQueue::Refresh)
                    {
                        if(m_timer > 0)
                            --m_timer;
                        else 
                        {
                            m_animation = true;
                            m_state = SelectAButton;
                            displayButton();
                        }
                        displayTimer();
                    }
                    else if(event == EventQueue::Button)
                    {
                        m_state = SelectAButton;
                        displayButton();
                    }
                    break;
            }
        }

        BackgroundState update()
        {
            
            if(m_watchdog.isCalibrating())
            {
                m_events.empty();
                return Active;
            }
            else if(!m_calibrated)
            {
                m_calibrated = true;

                displayTimer();
                displayButton();
            }

            EventQueue::EventType event = m_events.next();

            while(event != EventQueue::NoEvent)
            {      
                update(event);  
                event = m_events.next();
            }

            
            if(m_nbRefreshWithoutEvents > 10 && !m_animation)
                m_screenLight.stop();
            else 
                m_screenLight.start();

            if(m_nbRefreshWithoutEvents > 20 && !m_animation && m_state != StartButton)
            {
                if(!m_powerSave)
                {
                    m_u8x8.setPowerSave(1);
                    m_powerSave = true;
                }
            }
            else if(m_powerSave)
            {
                m_u8x8.setPowerSave(0);
                m_powerSave = false;
            }

            return m_animation ? Animation : m_nbRefreshWithoutEvents > 2 ? Sleep : Active;
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
        State          m_state;
        bool           m_calibrated;
        bool           m_animation;
        bool           m_powerSave;
};

#endif
