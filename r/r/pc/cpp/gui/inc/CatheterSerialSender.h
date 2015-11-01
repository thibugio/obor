/* SerialSender.h */

#ifndef CATHETER_SERIAL_SENDER_H
#define CATHETER_SERIAL_SENDER_H

#include <cstdint>
#include <string>
#include <stdint.h>

#include "common_utils.h"
#include "serial_utils.h"

class SerialSender {
    public:
        SerialSender();
        
        ~SerialSender();
        
        bool init();
        
        std::string showSerialPath();
        
        int sendByteVectors(std::vector<std::vector<uint8_t>> bytes, std::vector<int> delays);
        
        std::vector<std::vector<uint8_t>> CatheterSerialSender::sendByteVectorsAndRead(
            std::vector<std::vector<uint8_t>> pbytes,
            std::vector<int> pdelays);

    private:
        const std::string& serial_path;
        serial_port sp;
};
#endif
