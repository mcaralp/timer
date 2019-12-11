
#ifndef SCREEN_LIGHT_H_
#define SCREEN_LIGHT_H_

class ScreenLight
{
    public:
        ScreenLight()
        {
            pinMode(17, OUTPUT);
            stop();
        }

        void start()
        {
            digitalWrite(17, true);
        }

        void stop()
        {
            digitalWrite(17, false);
        } 

};

#endif
