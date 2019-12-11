
#include <avr/sleep.h>
#include <avr/power.h>

#include "app.h"

App app;

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

void loop()
{
    if(app.update())
    {
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();
    }
}
