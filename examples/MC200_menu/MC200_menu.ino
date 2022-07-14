//#include "Arduino.h"
#include "MC_200.h"


MC_200 my_motor;


void printMotorResponse(const MC_200 & motor){
    Serial.print("\nMotor Response:");
    motor.response.debugPrintMessage();
    Serial.println();
    Serial.print("Response, current: "); Serial.println(motor.currentAmps());
    Serial.print("Response, encoder pulse: "); Serial.println(motor.encoderPulses());
    Serial.print("Response, centiDegrees: "); Serial.println(motor.encoderCentiDegrees());
    Serial.print("Response, working Mode: "); Serial.println((int)motor.workingMode());
    Serial.print("Response, is turned on?: "); Serial.println(motor.isTurnedOn());
    if (motor.alarmLimitSPIError() & 0B00000001) Serial.println("SPI comm error");
    if (motor.alarmLimitSPIError() & 0B00000010) Serial.println("Limit alarm");
    if (motor.alarmLimitSPIError() & 0B00000100) Serial.println("Alarm status");
}


void setup(){
    Serial.begin(115200);
    SPI.begin();
    my_motor.initialize(10,5000000);
}


void loop(){

    if(Serial.available())
    {
        char my_char = Serial.read();    //Consume the received byte by serial.

        switch (my_char) {
        case '1':
            my_motor.turnOn();
            Serial.println("Turning On");
            printMotorResponse(my_motor);
            break;

        case '0':
            my_motor.turnOff();
            Serial.println("Turning OFF");
            printMotorResponse(my_motor);
            break;

        case 'P':
            while (!Serial.available());
            my_char = Serial.read();
            switch (my_char){
                case '0':
                    Serial.println("Sending Position 0 pulses");
                    my_motor.setPositionPulses(0);
                    break;

                case '1':
                    Serial.println("Sending Position 1296000 pulses");
                    my_motor.setPositionPulses(1296000);
                    break;

                case '2':
                    Serial.println("Sending Position -324000 pulses");
                    my_motor.setPositionPulses(-324000);
                    break;

                case '3':
                    Serial.println("Sending Position 1296000 centidegrees");
                    my_motor.setPositionCentiDegrees(36000);
                    break;

                case '4':
                    Serial.println("Sending Position 18000 centidegrees");
                    my_motor.setPositionCentiDegrees(18000);
                    break;

                case '5':
                    Serial.println("Sending Position -18000 centidegrees");
                    my_motor.setPositionCentiDegrees(-18000);
                    break;
            }
            printMotorResponse(my_motor);
            break;

        case 'C':
            while (!Serial.available());
            my_char = Serial.read();
            switch (my_char){
                case '0':
                    Serial.println("Sending Current 0 Amps");
                    my_motor.setCurrent(0.0);
                    break;

                case '1':
                    Serial.println("Sending Current 0.1 Amps");
                    my_motor.setCurrent(0.1);
                    break;

                case '2':
                    Serial.println("Sending Current 0.2 Amps");
                    my_motor.setCurrent(0.2);
                    break;

                case '5':
                    Serial.println("Sending Current 0.5 Amps");
                    my_motor.setCurrent(0.5);
                    break;

                case '6':
                    Serial.println("Sending Current -0.2 Amps");
                    my_motor.setCurrent(-0.2);
                    break;
            }
            printMotorResponse(my_motor);
            break;


        case 'M':
            while (!Serial.available());
            my_char = Serial.read();
            switch (my_char){
                case 'P':
                    Serial.println("Changing to position control mode");
                    my_motor.changeWorkingMode(MC_200::WorkingMode::position_mode);
                    break;

                case 'V':
                    Serial.println("Changing to velocity control mode");
                    my_motor.changeWorkingMode(MC_200::WorkingMode::velocity_mode);
                    break;

                case 'C':
                    Serial.println("Changing to current control mode");
                    my_motor.changeWorkingMode(MC_200::WorkingMode::current_mode);
                    break;
            }
            printMotorResponse(my_motor);
            break;

        case 'R':
            my_motor.updateStatus();
            printMotorResponse(my_motor);

        case 'X':
            printMotorResponse(my_motor);
            break;

        }
        Serial.println("\n\n");
    }


}