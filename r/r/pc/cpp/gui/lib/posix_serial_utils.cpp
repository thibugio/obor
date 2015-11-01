/* serial_utils.cpp */

#include "serial_utils.h"

#include <termios.h>
#include <unistd.h>

bool serialSendPacket(int fd, catheterPacket * p) {
    const size_t size = sizeof(catheterPacket) + p->cmds.size() * sizeof(catheterChannelCmd);
    return size == write(fd, p, size); //README: will this correctly handle struct w/ vector?
}

bool serialSendByteVec(int fd, std::vector<uint8_t> *bytes) {
    const size_t size = bytes.size() * sizeof(uint8_t);
    return size == write(fd, bytes, size);
}

void serialSetBaud(int fd, Baud b) {
    speed_t baud;
    switch (b) {
    case Baud::9600B:
        baud=B9600;
        break;
    case Baud::19200B:
        baud=B19200;
        break;
    case Baud::115200B:
        baud=B115200;
        break;
    }
    
    struct termios serial_attr;
    tcgetattr(fd, &serial_attr);
    cfsetospeed(&serial_attr, baud);
    tcsetattr(fd, TCSADRAIN, &serial_attr);
}

std::string getSerialPath() {
    std::string path;
    #ifdef UNX
        path = "/dev/tty"; 
        FILE *pipe = popen("ls /dev/tty* | egrep -o \"(ACM|USB)[0-9]\"", "r");
        if (!pipe) return false;
        char buf[8];
        while(!feof(pipe)) {
            if (fgets(buf, 8, pipe) != NULL)
                path += buf;
        }
        pclose(pipe);
    #else
        path = "COM9"; 
    #endif
    return path;
}
