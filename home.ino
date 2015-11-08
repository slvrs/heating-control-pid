#include <OLED_I2C.h>
extern uint8_t SmallFont[];
extern uint8_t BigNumbers[]; 
extern uint8_t TinyFont[];

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 10
#define TEMPERATURE_READ_INTERVAL 500
#define OLED_SENSOR_SWITCH_INTERVAL 9000


#define PUMP 4


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
OLED oled(SDA, SCL);


unsigned long lastSensorsRead;


void setup() {
  Serial.begin(9600);
  sensors.begin();
  oled.begin();
}

void loop() {
  readSensors();
  updateDisplayValue();

  
//  Serial.println(sensors.getDeviceCount());
//  for (int i = 0; i < sensors.getDeviceCount(); i++) {
//    Serial.println(sensors.getTempCByIndex(i));
//  }
//  Serial.println("Completed");
}

void readSensors() {
  if (millis() - lastSensorsRead < TEMPERATURE_READ_INTERVAL)
    return;
  sensors.requestTemperatures();

  lastSensorsRead = millis();
}

void updateDisplayValue() {
  static unsigned long lastSensorSwitch;
  static int sensorIndex;

  bool doUpdate = false;

  uint8_t sensorCount = sensors.getDeviceCount(); 

  if (millis() - lastSensorSwitch > OLED_SENSOR_SWITCH_INTERVAL) {
    lastSensorSwitch = millis();
    doUpdate = true;
    sensorIndex++;
    if (sensorIndex >= sensorCount) 
      sensorIndex = 0;
  }

  static unsigned long prevSensorsRead;
  if (prevSensorsRead != lastSensorsRead) {
    prevSensorsRead = lastSensorsRead;
    doUpdate = true;
  }

  if (doUpdate) {
    oled.clrScr();
    oled.setFont(SmallFont);
    oled.print("SENSOR " + String(sensorIndex + 1) + "/" + String(sensorCount), CENTER, 0);

    oled.setFont(BigNumbers);
    oled.print(String(sensors.getTempCByIndex(sensorIndex)), CENTER, 20);

    oled.update();    
}









}

