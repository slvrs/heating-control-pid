#include "pump.h"

Pump::Pump (int aPin) {
    pin = aPin;
};

void Pump::begin() {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
};

void Pump::on () {
    digitalWrite(pin, HIGH);
    onMillis = millis();
    isOn = true;
};

void Pump::off () {
    digitalWrite(pin, LOW);
    offMillis = millis();
    isOn = false;
};

bool Pump::getIsOn() {
    return isOn;
};

unsigned long Pump::getOnMillis() {
    return onMillis;
};

unsigned long Pump::getOffMillis() {
    return offMillis;
};
