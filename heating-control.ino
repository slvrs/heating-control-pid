#include <OLED_I2C.h>
extern uint8_t BigNumbers[];
extern uint8_t MegaNumbers[];

#include <OneWire.h>
#include <DallasTemperature.h>

#include "pump.h"

#define PIN_PUMP 13

#define PIN_ONE_WIRE_BUS 5
#define TEMPERATURE_READ_INTERVAL 750
#define TEMPERATURE_RESOLUTION 10

#define OLED_SENSOR_SWITCH_INTERVAL 5000

#define PIN_ALARM 6
#define ALARM_TEMPERATURE 85
#define ALARM_INTERVAL_MIN 250
#define ALARM_INTERVAL_RANGE 750

#define PUMP_ON_TEMP_DIFF 15
#define PUMP_ON_TEMP_MIN 58


OneWire oneWire(PIN_ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
OLED oled(SDA, SCL);
Pump pump(PIN_PUMP);

DeviceAddress sens1 =
    {0x28, 0x16, 0xd1, 0x2f, 0x04, 0x00, 0x00, 0x10};
DeviceAddress sens2 =
    {0x28, 0x5D, 0xB0, 0x2F, 0x04, 0x00, 0x00, 0x41};



unsigned long lastSensorsRead;


void setup() {
    Serial.begin(9600);
    sensors.begin();
    oled.begin();
    pump.begin();
    pinMode(PIN_ALARM, OUTPUT);

    sensors.setResolution(TEMPERATURE_RESOLUTION);
//    sensors.setWaitForConversion(false);
}

void loop() {
    readSensors();
    updateDisplay();
    controlPump();
    ringHighTempAlarm();

    delay(1);
}

void ringHighTempAlarm() {
    static unsigned long nextWrite = 0;
    static bool lastState;

    bool doWrite = millis() > nextWrite;
    if (doWrite) {
        int x = rand() % ALARM_INTERVAL_RANGE;
        nextWrite = millis() + ALARM_INTERVAL_MIN + x;

        int t1 = (sensors.getTemp(sens1)>>7);
        bool doAlarm = t1 >= ALARM_TEMPERATURE;


        lastState = doAlarm && !lastState;

        if (lastState)
            digitalWrite(PIN_ALARM, HIGH);
        else
            digitalWrite(PIN_ALARM, LOW);
    }
}

void readSensors() {
    static unsigned long nextRead = 0;
    if (millis() < nextRead)
        return;
    sensors.requestTemperatures();

    lastSensorsRead = millis();
    nextRead = lastSensorsRead + TEMPERATURE_READ_INTERVAL;
        //max(TEMPERATURE_READ_INTERVAL, 750 >> (12 - TEMPERATURE_RESOLUTION));
}

void updateDisplay() {
    static unsigned long lastSensorSwitch;
    static int index;

    bool doUpdate = false;

    uint8_t count = sensors.getDeviceCount();

    if (millis() - lastSensorSwitch > OLED_SENSOR_SWITCH_INTERVAL) {
        lastSensorSwitch = millis();
        doUpdate = true;
        index++;
        if (index >= count)
            index = 0;
    }

    static unsigned long prevSensorsRead;
    if (prevSensorsRead != lastSensorsRead) {
        prevSensorsRead = lastSensorsRead;
        doUpdate = true;
    }

    if (doUpdate) {
        oled.clrScr();
        oled.setFont(BigNumbers);
        oled.print(String(index + 1),
                70,
                0);

        oled.setFont(MegaNumbers);
        oled.print(String(sensors.getTempCByIndex(index), 1),
                CENTER,
                25);

        oled.update();
    }
}

void controlPump() {
    static unsigned long lastWrite = 0;

    if (millis() - lastWrite < 300)
        return;

    lastWrite = millis();

    bool on = false;

    int t1 = (sensors.getTemp(sens1)>>7);
    int t2 = (sensors.getTemp(sens2)>>7);

    on = ((t1 - t2) > PUMP_ON_TEMP_DIFF) ||
        (t1 >= PUMP_ON_TEMP_MIN);

    if (on) {
        if (!pump.getIsOn() && ((millis() - pump.getOffMillis()) > 10000))
            pump.on();
    }
    else
        if (pump.getIsOn() && ((millis() - pump.getOnMillis()) > 60000))
            pump.off();
}

