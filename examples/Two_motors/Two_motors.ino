#include <SPI.h>
#include "MC_200.h"

#define CS_MOTOR_0 10
#define CS_MOTOR_1 9

MC_200 my_motor_0;
MC_200 my_motor_1;


void setup() {
    Serial.begin(115200);
    SPI.begin();

    my_motor_0.initialize(CS_MOTOR_0);
    my_motor_1.initialize(CS_MOTOR_1);

    my_motor_0.turnOn();
    my_motor_1.turnOn();
}


void loop() {
    my_motor_0.setCurrent(0);
    my_motor_1.setCurrent(0);

    Serial.print(my_motor_0.encoderCentiDegrees()); Serial.print('\t');
    Serial.print(my_motor_1.encoderCentiDegrees()); Serial.print('\t');
    Serial.println();

    delay(50);
}