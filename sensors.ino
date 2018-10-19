#include <OneWire.h>
#include <DallasTemperature.h>

#define PIN_ONE_WIRE_BUS 5
#define TEMPERATURE_READ_INTERVAL 750
#define TEMPERATURE_RESOLUTION 12

OneWire oneWire(PIN_ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const DeviceAddress sens1 =
  {0x28, 0x16, 0xD1, 0x2F, 0x04, 0x00, 0x00, 0x10};
const DeviceAddress sens2 =
  {0x28, 0x5D, 0xB0, 0x2F, 0x04, 0x00, 0x00, 0x41};

// values two temperature sensors
float tempHigh, tempLow;
// last time of update request
unsigned long requestMillis;
bool highUpdated, lowUpdated;

void sensors_init() {
  sensors.begin();
  sensors.setResolution(TEMPERATURE_RESOLUTION);
  sensors.setWaitForConversion(false);
  requestTemperatures();
}

float sensors_getHigh() {
  return tempHigh;
}

float sensors_getLow() {
  return tempLow;
}

void sensors_process() {
  if (millis() - requestMillis < (768 >> (12 - TEMPERATURE_RESOLUTION)))
    return;

  if (!highUpdated) {
    tempHigh = sensors.getTempC(sens1);
    highUpdated = true;
    return;
  }

  if (!lowUpdated) {
    tempLow = sensors.getTempC(sens2);
    lowUpdated = true;
    return;
  }
  
  requestTemperatures();
  highUpdated = false;
  lowUpdated = false;
}

void requestTemperatures() {
  sensors.requestTemperatures();
  requestMillis = millis();
}
