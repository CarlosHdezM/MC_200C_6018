#include "MC_200.h"
#include "MC_200_Message.h"



MC_200::MC_200() : m_CS_pin(M_DEFAULT_CS_PIN) , m_SPI_speed(M_DEFAULT_SPI_SPEED)
{
}



bool MC_200::initialize(uint8_t cs_pin, uint32_t spi_speed)
{
    m_CS_pin = cs_pin;
    m_SPI_speed = spi_speed;
    m_last_SPI_transfer_usec = micros();
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
    return true;
}



void MC_200::m_multiTransferMC200Message(MC_200_Message & msg, uint8_t msg_count)
{
    m_initSPITransfer();
    for (uint8_t i = 0; i < msg_count; i++)         
    {
        m_transferMC200Message(msg);
    }
    m_endTransferSPIMessage();
}


/*Returns 1 if the motor succeeded in turning on.*/
bool MC_200::turnOn()
{
    MC_200_Message message_to_send;
    message_to_send.data[0] = 0B00001000;
    m_multiTransferMC200Message(message_to_send, 5);   //ToDo: Check this magic count number
    if(m_response_ok) { m_updateStatusVariables(); }
    return m_is_turned_on;
}


/*Returns 1 if the motor succeeded in turning off.*/
bool MC_200::turnOff()
{
    MC_200_Message message_to_send;  //Defaults with all Zeros.
    m_multiTransferMC200Message(message_to_send, 7);    //ToDo: Check this magic count number
    if (m_response_ok) { m_updateStatusVariables(); }
    return !m_is_turned_on;
}


void MC_200::m_initSPITransfer()
{
    SPI.beginTransaction(SPISettings(m_SPI_speed, MSBFIRST, SPI_MODE3));
}



bool MC_200::m_transferMC200Message(MC_200_Message & msg)
{
    msg.calculateChecksum();
    #if DEBUG_ENABLED
    Serial.print("Transfering: "); msg.debugPrintMessage(); Serial.println();
    #endif
    //Wait the mandatory 35 microseconds between each 12 bytes transfer. 
    long delay_needed = 45 - (micros() - m_last_SPI_transfer_usec);     //ToDo: Check and adjust this delay. Also take care of the magic number.
    if (delay_needed > 0) delayMicroseconds(delay_needed);

    digitalWrite(m_CS_pin,LOW);
    delayMicroseconds(20);                                               //ToDo: Check and adjust this delay. Also take care of the magic number.
    for (uint8_t i = 0; i < MC_200_Message::MESSAGE_SIZE_BYTES; i++)
    {
        response.data[i] = SPI.transfer(msg.data[i]);
    }
    m_last_SPI_transfer_usec = micros(); 
    digitalWrite(m_CS_pin, HIGH);
    //Check for message received integrity (CRC)
    uint8_t CRC_LOW =  response.data[MC_200_Message::MESSAGE_SIZE_BYTES - 2];
    uint8_t CRC_HIGH = response.data[MC_200_Message::MESSAGE_SIZE_BYTES - 1];
    response.calculateChecksum();
    m_response_ok = (CRC_LOW == response.data[MC_200_Message::MESSAGE_SIZE_BYTES - 2] and CRC_HIGH == response.data[MC_200_Message::MESSAGE_SIZE_BYTES - 1]);
    return m_response_ok;
}



void MC_200::m_endTransferSPIMessage()
{
    SPI.endTransaction();
}



bool MC_200::m_transferDummyMessage(){
    MC_200_Message message_to_send = MC_200_Message();
    message_to_send.data[0] = 0B11111000;
    return m_transferMC200Message(message_to_send);
}



bool MC_200::setPositionCentiDegrees(int32_t centi_degrees){
    return setPositionPulses(m_centiDegreesToPulses(centi_degrees));
}


bool MC_200::setPositionPulses(int32_t pulse_position) 
{
    if(!m_is_turned_on or m_working_mode != WorkingMode::position_mode or m_alarm_limit_spi_error){
        m_response_ok = false;
        return false;
    }
    MC_200_Message message_to_send;
    //Prepare the message.
    message_to_send.data[0] = 0B00001000;
    message_to_send.data[3] = pulse_position;
    message_to_send.data[4] = pulse_position >> 8;
    message_to_send.data[5] = pulse_position >> 16;
    message_to_send.data[6] = pulse_position >> 24;
    //Sending a message of 12 bytes. 
    m_initSPITransfer();
    m_transferMC200Message(message_to_send);
    m_endTransferSPIMessage();
    if (m_response_ok){ m_updateStatusVariables(); }
    return m_response_ok;
}


bool MC_200::setCurrent(float curr_setpoint)
{
    if(!m_is_turned_on or m_working_mode != WorkingMode::current_mode or m_alarm_limit_spi_error){
        m_response_ok = false;
        return false;
    }
    union4bytes current_union;
    current_union.value_float = curr_setpoint;
    //Prepare the message
    MC_200_Message message_to_send;
    message_to_send.data[0] = 0B00001000;
    message_to_send.data[3] = current_union.bytes[0];
    message_to_send.data[4] = current_union.bytes[1];
    message_to_send.data[5] = current_union.bytes[2];
    message_to_send.data[6] = current_union.bytes[3];
    //Sending a message of 12 bytes. 
    m_initSPITransfer();
    m_transferMC200Message(message_to_send);
    m_endTransferSPIMessage();
    if (m_response_ok){ m_updateStatusVariables(); }
    return m_response_ok;
}


MC_200::WorkingMode MC_200::changeWorkingMode(const MC_200::WorkingMode & working_mode){
    MC_200_Message message_to_send;
    message_to_send.data[0] = 0B00001000 | (uint8_t)working_mode;       //HERE! Maybe later instead of 1, check whether the motor is turned on or off and or that bit too. 
    message_to_send.data[3] = 0X0F;
    message_to_send.data[4] = 0X0F;
    message_to_send.data[5] = 0X0F;
    message_to_send.data[6] = 0X0F;
    message_to_send.debugPrintMessage();
    m_initSPITransfer();
    m_transferMC200Message(message_to_send);
    message_to_send.data[3] = 0XF0;
    message_to_send.data[4] = 0XF0;
    message_to_send.data[5] = 0XF0;
    message_to_send.data[6] = 0XF0;
    message_to_send.debugPrintMessage();
    m_transferMC200Message(message_to_send);
    message_to_send.data[3] = 0X00;
    message_to_send.data[4] = 0X00;
    message_to_send.data[5] = 0X00;
    message_to_send.data[6] = 0X00;
    message_to_send.debugPrintMessage();
    m_transferMC200Message(message_to_send);
    m_transferDummyMessage();
    m_transferDummyMessage();
    m_endTransferSPIMessage();
    m_updateStatusVariables();
    return m_working_mode;
}


/*From data member "response", updates the motor status variables:
m_encoder_pulses, m_encoder_deg, m_current_amps, m_working_mode, m_is_turned_on;*/
void MC_200::m_updateStatusVariables(){
    m_current_amps = m_responseDecodeCurrent();
    m_encoder_pulses = m_responseDecodeEncoderPulses();
    m_encoder_centidegrees = m_pulsesToCentiDegrees(m_encoder_pulses);
    m_working_mode = (WorkingMode)(response.data[9] & 0B00001111);
    m_is_turned_on = response.data[0] & 0B00001000;
    m_alarm_limit_spi_error = response.data[0] & 0B00000111;
    return;
}


float MC_200::m_responseDecodeCurrent(){
    union4bytes pulses_data;
    pulses_data.bytes[0] = response.data[5];
    pulses_data.bytes[1] = response.data[6];
    pulses_data.bytes[2] = response.data[7];
    pulses_data.bytes[3] = response.data[8];
    return pulses_data.value_float;
}


int32_t MC_200::m_responseDecodeEncoderPulses(){
    union4bytes pulses;
    pulses.bytes[0] = response.data[1];
    pulses.bytes[1] = response.data[2];
    pulses.bytes[2] = response.data[3];
    pulses.bytes[3] = response.data[4];
    return pulses.value_int32;
}


inline int32_t MC_200::m_pulsesToCentiDegrees(int32_t pulses){
    return m_encoder_pulses / 36; 
    /*
        ToDo: Change this magic number for constants and add the appropriate member variables.
            1,296,000 -> 360 degrees.
            1,296,000 = (9)(4)(36000)
            Reduction          -> 9
            Encoder resolution -> 36000
            ???                -> 4
    */
}

inline int32_t MC_200::m_centiDegreesToPulses(int32_t centi_degrees){
    return centi_degrees * 36;
    /*
        ToDo: Same as in  m_pulsesToCentiDegrees, take care of the magic number 36. 
    */
}



void MC_200::updateStatus()                                                //EXPERIMENTAL! Not ready to be used yet.
{
    m_initSPITransfer();
    m_transferDummyMessage();
    m_endTransferSPIMessage();
    m_updateStatusVariables();
}