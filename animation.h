
#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <toneAC.h>

#include "leds.h"
#include "music.h"

class Animation
{
    public: 
        static constexpr uint8_t Red   = 3;
        static constexpr uint8_t Green = 5;
        static constexpr uint8_t Blue  = 6;

        Animation(const Leds & leds, const Music & music)
            : m_isStarted(false), m_last(0), 
              m_leds(leds), m_ledsCurrentDuration(0), m_ledsCurrentPos(0),
              m_music(music), m_musicCurrentDuration(0), m_musicCurrentPos(0)
        {
            m_ledsPeriod = pgm_read_word_near(&m_leds.period);
            m_ledsSize   = pgm_read_byte_near(&m_leds.size);

            m_musicBpm   = pgm_read_word_near(&m_music.bpm);
            m_musicSize  = pgm_read_byte_near(&m_music.size);
            m_musicNote  = (60000 * 4) / m_musicBpm;
        }

        bool isStarted() const
        {
            return m_isStarted;
        }

        void start()
        {
            if(m_isStarted) return;

            m_isStarted = true;
            m_last = millis();
            m_ledsCurrentDuration = 0;
            m_ledsCurrentPos = 0;
            m_musicCurrentDuration = 0;
            m_musicCurrentPos = 0;

            digitalWrite(Red, 1);
            digitalWrite(Green, 1);
            digitalWrite(Blue, 1);

            pinMode(Red,   OUTPUT);
            pinMode(Green, OUTPUT);
            pinMode(Blue,  OUTPUT);

            playColor();
            playNote();

        }

        void stop()
        {
            
            if(!m_isStarted) return;

            m_isStarted = false;

            digitalWrite(Red, 1);
            digitalWrite(Green, 1);
            digitalWrite(Blue, 1);

            pinMode(Red,   INPUT);
            pinMode(Green, INPUT);
            pinMode(Blue,  INPUT);

            toneAC();
        }

        void update()
        {
            if(!m_isStarted) 
                start();

            uint32_t now = millis();
            uint32_t elapsed = now - m_last;

            m_last = now;

            updateLeds(elapsed);
            updateMusic(elapsed);    
        }

        void updateLeds(uint32_t elapsed)
        {
            m_ledsCurrentDuration += elapsed;
            if(m_ledsCurrentDuration < m_ledsPeriod) return;
            m_ledsCurrentDuration -= m_ledsPeriod;
            
            playColor();
        }

        void updateMusic(uint32_t elapsed)
        {
            m_musicCurrentDuration += elapsed;
            if(m_musicCurrentDuration < m_musicNoteDuration) return;
            m_musicCurrentDuration -= m_musicNoteDuration;

            playNote();

        }

        void playColor()
        {
            uint8_t red   = pgm_read_byte_near(m_leds.data[m_ledsCurrentPos] + 0);      
            uint8_t green = pgm_read_byte_near(m_leds.data[m_ledsCurrentPos] + 1);  
            uint8_t blue  = pgm_read_byte_near(m_leds.data[m_ledsCurrentPos] + 2);   

            analogWrite(Red,   255 - red);
            analogWrite(Green, 255 - green);
            analogWrite(Blue,  255 - blue);

            if(++m_ledsCurrentPos == m_ledsSize)
                m_ledsCurrentPos = 0;  
        }

        void playNote()
        {
            
            int16_t len  = pgm_read_word_near(m_music.data[m_musicCurrentPos] + 1);
            int16_t freq = pgm_read_word_near(m_music.data[m_musicCurrentPos] + 0);   

            if (len > 0) 
            {
                m_musicNoteDuration = (m_musicNote) / len;
            } 
            else if (len < 0) 
            {
                m_musicNoteDuration = m_musicNote / -len * 1.5;
            }
            else 
            {
                m_musicNoteDuration = 0;
            }

            Serial.print(m_musicNoteDuration);
            Serial.print(' ');
            Serial.print(freq);
            Serial.print(' ');
            Serial.println(len);

            if(m_musicNoteDuration != 0)
                toneAC(freq, 10, m_musicNoteDuration, true);
            else 
                toneAC();

            if(++m_musicCurrentPos == m_musicSize)
                m_musicCurrentPos = 0;  
        }

    private: 
        bool          m_isStarted;
        uint32_t      m_last;

        const Leds &  m_leds;
        uint32_t      m_ledsCurrentDuration;
        uint32_t      m_ledsCurrentPos;
        uint32_t      m_ledsPeriod;
        uint32_t      m_ledsSize;

        const Music & m_music;
        uint32_t      m_musicCurrentDuration;
        uint32_t      m_musicCurrentPos;
        uint32_t      m_musicBpm;
        uint32_t      m_musicSize;
        uint32_t      m_musicNote;
        uint32_t      m_musicNoteDuration;
};

#endif 