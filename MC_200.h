#pragma once

#include <SPI.h>
#include "MC_200_Message.h"

#define DEBUG_ENABLED 0

class MC_200{

    public:
        //Public data members        
        MC_200_Message response;

        //Public user-defined data types
        enum class WorkingMode {position_mode = 1, velocity_mode = 2, current_mode = 3};

        //Public member functions
        MC_200(SPIClass& spi = SPI);
        bool initialize(uint8_t cs_pin = M_DEFAULT_CS_PIN, uint32_t spi_speed = M_DEFAULT_SPI_SPEED);
        bool turnOn(); 
        bool turnOff();
        bool setPositionPulses(int32_t pulse_position);
        bool setPositionCentiDegrees(int32_t centi_degrees);
        bool setCurrent(float curr_setpoint);
        WorkingMode changeWorkingMode(const WorkingMode & working_mode);       //EXPERIMENTAL! Not ready to be used yet.
        //"Getters" to the status variables.
        const int32_t & encoderPulses() const { return m_encoder_pulses;}
        const int32_t & encoderCentiDegrees() const { return m_encoder_centidegrees;}
        const float & currentAmps() const { return m_current_amps;}
        const WorkingMode & workingMode() const { return m_working_mode;}
        const bool & isTurnedOn() const { return m_is_turned_on;}
        const uint8_t & alarmLimitSPIError() const { return m_alarm_limit_spi_error;}
        const bool & isResponseOk() const {return m_response_ok;}
        void updateStatus();                                                //EXPERIMENTAL! Not ready to be used yet.


    private:
        //Class private constants.
        static constexpr uint8_t M_DEFAULT_CS_PIN = 10;
        static constexpr uint32_t M_DEFAULT_SPI_SPEED = 5000000;

        union union4bytes
        {
            int32_t value_int32;
            byte bytes[4];
            float value_float;
        };

        //Private member variables.
        uint8_t m_CS_pin;
        uint32_t m_SPI_speed;
        unsigned long m_last_SPI_transfer_usec;

        //Motor status variables
        bool m_response_ok;
        float m_current_amps;
        int32_t m_encoder_pulses;
        int32_t m_encoder_centidegrees;
        WorkingMode m_working_mode;
        bool m_is_turned_on;
        uint8_t m_alarm_limit_spi_error;

        //Private member functions.
        //----SPI Transfer Functions
        void m_initSPITransfer();
        bool m_transferMC200Message(MC_200_Message & msg);     //Response updates "response" data member. Returns true if the checksum is correct. 
        void m_endTransferSPIMessage();
        void m_multiTransferMC200Message(MC_200_Message & msg, uint8_t msg_count);
        //----Update member variables functions. 
        void m_updateStatusVariables();
        float m_responseDecodeCurrent();
        int32_t m_responseDecodeEncoderPulses();
        int32_t m_pulsesToCentiDegrees(int32_t pulses);
        int32_t m_centiDegreesToPulses(int32_t centi_degrees);

        bool m_transferDummyMessage();      //EXPERIMENTAL!
        SPIClass& m_spi;
};

        /*#ToDo:
            Implement member functions: 
                setPosition(int32_t position);  ---> This function will update the data members position (int32), current(int32)

                const MC_200_Message & updateReadings();     

                bool turnOn();
        */
 