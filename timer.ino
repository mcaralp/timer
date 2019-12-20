
#include <avr/sleep.h>
#include <avr/power.h>

#include "app.h"
#include "animation.h"

#include "ledsHSV.h"
#include "musicBip.h"

App app;
Animation anim(ledsHSV, musicBip);

ISR(WDT_vect)
{
    app.updateWatchdog();
}

ISR(PCINT2_vect) 
{
    app.updateEncoder();
}

void setup()
{
    Serial.begin(115200);

    app.init();
}

int cpt = 0;

void loop()
{
    switch(app.update())
    {
        case App::Sleep:
            anim.stop();
            set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_mode();
            break;

        case App::Animation:
            anim.update();
            break;

        case App::Active:
            anim.stop();
            break;
    }
}
