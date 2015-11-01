/* posix_serial_utils.h */
#ifndef POSIX_SERIAL_UTILS_H
#define POSIX_SERIAL_UTILS_H

#include "common_utils.h"
#include <cstdint>
#include <cstdio>

bool serialSendPacket(int fd, catheterPacket *p);

bool serialSendByteVec(int fd, std::vector<uint8_t> *bytes);

bool serialSetBaud(int fd, int b);

std::string getSerialPath();

#endif
