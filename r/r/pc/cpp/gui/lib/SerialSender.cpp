/* SerialSender.cpp */

#include "SerialSender.h"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace boost::asio;

SerialSender::SerialSender() 
: io(), sp(io) {
    serial_path = getSerialPath();
}

SerialSender::~SerialSender() {
    if (sp.is_open()) 
        sp.close();
}

void SerialSender::setBaud(Baud b) {
    unsigned int baud = static_cast<int>(b);
    sp.set_option(boost::asio::serial_port_base::baud_rate(baud));
}

std::string SerialSender::getSerialPath() {
    std::string path;
    #ifdef WINDOWS 
        path = "COM9"; 
    #else
        path = "/dev/tty"; 
        FILE *pipe = popen("ls /dev/tty* | egrep -o \"(ACM|USB)[0-9]\" | tr -d '\n'", "r");
        if (!pipe) return false;
        char buf[8];
        while(!feof(pipe)) {
            if (fgets(buf, 8, pipe) != NULL)
                path += buf;
        }
        pclose(pipe);
    #endif
    return path;
}

std::string SerialSender::showSerialPath() {
    return serial_path;
}

bool SerialSender::connect(Baud b) {
    boost::system::error_code ec;
    sp.open(serial_path, ec);
    if (!ec) {
        SerialSender::setBaud(b);
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
    }
    return !ec;
}

bool SerialSender::disconnect() {
    boost::system::error_code ec;
    sp.close(ec);
    return !ec;
}

bool SerialSender::sendByteVector(std::vector<uint8_t> bytes) {
    size_t size_exp = bytes.size() * sizeof(uint8_t);
    //size_t size_ret = boost::asio::write(sp, boost::asio::buffer(bytes, bytes.size()));
    size_t size_ret = sp.write_some(boost::asio::buffer(bytes, bytes.size()));
    return size_ret == size_exp;
}

size_t SerialSender::sendByteVectorAndRead(std::vector<uint8_t> bytes, 
                                              std::vector<uint8_t>& bytesRead, size_t maxBytesToRead) {
    if (!(SerialSender::sendByteVector(bytes)))
        return 0;
    bytesRead.clear();
    size_t nbytes=0;
    uint8_t b;
    while(1) {
        //boost::asio::read(sp, boost::asio::buffer(&b, 1));
        sp.read_some(boost::asio::buffer(&b, 1));
        switch ((char)b) {
        case '\r': 
            break;
        case '\n': 
            return nbytes;
        default: 
            bytesRead.push_back((uint8_t)b);
            nbytes++;
        }
    }
    return nbytes;
}

bool SerialSender::sendByteVectors(std::vector<std::vector<uint8_t>> byteVecs, std::vector<int> delays) {
    if (byteVecs.size() != delays.size()) return false;

    unsigned int complete = 0;
    for (unsigned int i=0; i<byteVecs.size(); i++) {
        complete += SerialSender::sendByteVector(byteVecs[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(delays[i]));
    }
    return complete == byteVecs.size();
}

size_t SerialSender::sendByteVectorsAndRead(std::vector<std::vector<uint8_t>> byteVecs,
                                            std::vector<std::vector<uint8_t>>& bytesRead,
                                            std::vector<int> delays) {
    size_t nbytes=0;
    size_t npackets = byteVecs.size();
    bytesRead.clear();
    std::vector<uint8_t> retBytes;
    for (unsigned int i=0; i<npackets; i++) {
        nbytes += SerialSender::sendByteVectorAndRead(byteVecs[i], retBytes, byteVecs[i].size());
        bytesRead.push_back(retBytes);
        std::this_thread::sleep_for(std::chrono::milliseconds(delays[i]));
    }
    return nbytes;
}

void SerialSender::handler(const boost::system::error_code& ec, std::size_t bytes_transferred) {}
