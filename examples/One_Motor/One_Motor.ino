#include <SPI.h>
#include "MC_200.h"

#define CS_MOTOR_0 10

MC_200 my_motor_0;


void setup() {
    delay(1000);
    Serial.begin(115200);
    SPI.begin();

    my_motor_0.initialize(CS_MOTOR_0);

    while(!my_motor_0.turnOn()){
        Serial.println("Couldn't turn on the motor, retrying...");
        delay(100);
    }
}


void loop() {
    my_motor_0.setCurrent(0);

    if(my_motor_0.isResponseOk()){
        Serial.println(my_motor_0.encoderCentiDegrees());        
    }
    else{
        Serial.println("Error in response checksum validation, check SPI connections.");
    }
}
