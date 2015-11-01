/* SerialSender.h */

#pragma once

#ifndef SERIAL_SENDER_H
#define SERIAL_SENDER_H

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>

#include "common_utils.h"

using namespace boost::asio;

enum class Baud {
    BR_9600 = 9600,
    BR_19200 = 19200,
    BR_115200 = 115200
};

class SerialSender {
    public:
        SerialSender();
        
        ~SerialSender();
        
        std::string getSerialPath();
        
        void setBaud(Baud b);

        std::string showSerialPath();
        
        bool connect(Baud b);
        
        bool disconnect();
        
        bool sendByteVector(std::vector<uint8_t> bytes);

        size_t sendByteVectorAndRead(std::vector<uint8_t> bytes, std::vector<uint8_t>& bytesRead, 
                                     size_t maxBytesToRead);

        bool sendByteVectors(std::vector<std::vector<uint8_t>> bytes, std::vector<int> delays);
        
        size_t sendByteVectorsAndRead(std::vector<std::vector<uint8_t>> bytes,
            std::vector<std::vector<uint8_t>>& bytesRead, std::vector<int> delays);
        
        static void handler(const boost::system::error_code& ec, std::size_t bytes_transferred);
    private:
        std::string serial_path;
        io_service io;
        serial_port sp;
};
#endif
