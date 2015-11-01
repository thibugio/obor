/* CatheterSerialSender.cpp */

#include "CatheterSerialSender.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <iostream>

CatheterSerialSender::CatheterSerialSender() {}

bool CatheterSerialSender::init() {
    serial_path = getSerialPath();
    serial_fd = open(serial_path.c_str(), O_WRONLY);
    Baud b = Baud::9600B;
    serialSetBaud(serial_fd, baud_rate);
    return serial_fd >= 0;
}

CatheterSerialSender::~CatheterSerialSender() {
    close(serial_fd);
}

int CatheterSerialSender::sendPackets(std::vector<struct catheterPacket> packets) {
    int ret=0;
    for (int i=0; i<packets.size(); i++) {
        ret += serialSendPacket(serial_fd, &(packets[i]));        
    }
    return ret;
}

int CatheterSerialSender::sendByteVectors(std::vector<std::vector<uint8_t>> byteVecs, std::vector<int> d) {
    int ret=0;
    for (int i=0; i<byteVecs.size(); i++) {
        ret += serialSendByteVec(serial_fd, &(byteVecs[i]));
        std::this_thread::sleep_for(std::chrono::milliseconds(d[i]));
    }
    return ret;
}

std::vector<std::vector<uint8_t>> CatheterSerialSender::sendByteVectorsAndRead(
    std::vector<std::vector<uint8_t>> pbytes,
    std::vector<int> pdelays) {
    
    std::vector v();
    std::vector<std::vector<uint8_t>> bytesRead(pbytes.size(), v);
    int ret=0;
    for (int i=0; i<byteVecs.size(); i++) {
        serialSendByteVec(serial_fd, &(byteVecs[i]));
        std::this_thread::sleep_for(std::chrono::milliseconds(d[i]));
        unsigned char c;
        while(read(serial_fd, &c, 1) > 0) {
            bytesRead[i].push_back(c);
        }
    }
    return bytesRead;
}

std::string getPath() {
    return serial_path;
}
