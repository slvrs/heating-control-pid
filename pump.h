#ifndef _PUMP_
#define _PUMP_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

class Pump {
        int pin;
        unsigned long onMillis;
        unsigned long offMillis;
        bool isOn;
    public:
        void begin();
        Pump(int aPin);
        void on ();
        void off ();
        bool getIsOn();
        unsigned long getOnMillis();
        unsigned long getOffMillis();
};
#endif
