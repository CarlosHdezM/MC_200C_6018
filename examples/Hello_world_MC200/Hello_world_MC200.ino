#include <SPI.h>
#include "MC_200.h"

MC_200 my_motor;

void setup() {
  SPI.begin();
  Serial.begin(115200);
  my_motor.initialize(10);
  if (my_motor.turnOn()){ 
    Serial.println("Motor turned on succesfully..."); 
  }
  else{
    Serial.println("Couldn't turn on the motor. Aborting.");
    while(1){}
  }
}


void loop() {

  Serial.println("Setting Current to 0.3 Amps");
  while(my_motor.encoderCentiDegrees() < 72000){
    my_motor.setCurrent(0.3);
    Serial.print("Position (Centidegrees): "); Serial.println(my_motor.encoderCentiDegrees());
    Serial.print("Current (Amperes): "); Serial.println(my_motor.currentAmps());
  }

  Serial.println("Setting Current to 0 Amps");
  my_motor.setCurrent(0.0);
  delay(500);

  Serial.println("Setting Current to -0.3 Amps");
  while(my_motor.encoderCentiDegrees() > 0){
    my_motor.setCurrent(-0.3);
    Serial.print("Position (Centidegrees): "); Serial.println(my_motor.encoderCentiDegrees());
    Serial.print("Current (Amperes): "); Serial.println(my_motor.currentAmps());
  }

}
