/* posix_serial_utils.h */
#ifndef POSIX_SERIAL_UTILS_H
#define POSIX_SERIAL_UTILS_H

#include "common_utils.h"
#include <cstdint>
#include <cstdio>

enum class Baud {
    9600B,
    19200B,
    115200B
};

bool serialSendPacket(int fd, catheterPacket *p);

bool serialSendByteVec(int fd, std::vector<uint8_t> *bytes);

void serialSetBaud(int fd, Baud b);

std::string getSerialPath();

#endif
