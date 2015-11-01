/* serial_utils.h */
#ifndef SERIAL_UTILS_H
#define SERIAL_UTILS_H

#include <cstdint>
#include <cstdio>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
//#include <boost/thread.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>

#include "common_utils.h"

enum class Baud {
    9600B,
    19200B,
    115200B
};

bool serialSendByteVec(int fd, std::vector<uint8_t> *bytes);

void serialSetBaud(int fd, Baud b);

std::string getSerialPath();

#endif
