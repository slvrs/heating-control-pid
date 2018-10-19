#include <OLED_I2C.h>
extern uint8_t BigNumbers[];
extern uint8_t MegaNumbers[];

#define OLED_SENSOR_SWITCH_INTERVAL 5000

OLED oled(SDA, SCL);

void setup() {
  Serial.begin(9600);
  sensors_init();
  oled.begin();
}

void loop() {
  updateDisplay();
  sensors_process();
}

void updateDisplay() {
  static unsigned long lastSensorSwitch;
  static bool highSensor;
  float temperature;
  static float prevTemperature;
  
  bool doUpdate = false;
  
  if (millis() - lastSensorSwitch > OLED_SENSOR_SWITCH_INTERVAL) {
    lastSensorSwitch = millis();
    highSensor = !highSensor;
  }
  
  if (highSensor) {
    temperature = sensors_getHigh();
  } else {
    temperature = sensors_getLow();
  }
  
  if (temperature != prevTemperature) {
    doUpdate = true;
    prevTemperature = temperature;
  }
  
  if (doUpdate) {
    oled.clrScr();
    oled.setFont(BigNumbers);
    oled.print(String(highSensor ? 1 : 2),
      70,
      0);
    oled.setFont(MegaNumbers);
    oled.print(String(temperature, 1),
      CENTER,
      25);
    oled.update();
  }
}
