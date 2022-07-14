#include "MC_200_Message.h"


void MC_200_Message::calculateChecksum()
{
    uint32_t checksum_32 = 0;
    for (uint8_t i = 0; i < MC_200_Message::MESSAGE_SIZE_BYTES - 2; i+=2)       //The loop stops at 10  (MESSAGE_SIZE_BYTES - 2) because the last 2 bytes are used to store the checksum, and aren't part of the calculation.
    {
        checksum_32 += data[i] + (data[i + 1] << 8);
    }
    uint16_t checksum_16 = (uint16_t)checksum_32;
    checksum_16 |= 1 << 15;
    data[MC_200_Message::MESSAGE_SIZE_BYTES - 1] = checksum_16 >> 8;
    data[MC_200_Message::MESSAGE_SIZE_BYTES - 2] = checksum_16;
}


void MC_200_Message::debugPrintMessage() const
{
    //#if DEBUG_ENABLED
    Serial.print("[");
    for (uint8_t i = 0; i < MESSAGE_SIZE_BYTES; i++)
    {
        Serial.print("0x");
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.print("]");
    //#endif
}
