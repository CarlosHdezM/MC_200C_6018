#pragma once
#include "Arduino.h"

class MC_200_Message{
    private:

    public:
        //Class constants
        static constexpr uint8_t MESSAGE_SIZE_BYTES = 12;

        //Public data members
        uint8_t data[MESSAGE_SIZE_BYTES];

        //Public member functions
        MC_200_Message(): data({}) {}           //Constructor, initilizes array with all 0's
        void calculateChecksum();
        void debugPrintMessage() const;
};
