/* posix_serial_utils.cpp */

#include "posix_serial_utils.h"

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

bool serialSetBaud(int fd, int b) {
    speed_t baud;
    switch (b) {
    case 9600:
        baud=B9600;
        break;
    case 19200:
        baud=B19200;
        break;
    case 115200:
        baud=B115200;
        break;
    default: return false;
    }
    
    struct termios serial_attr;
    tcgetattr(fd, &serial_attr);
    cfsetospeed(&serial_attr, baud);
    tcsetattr(fd, TCSADRAIN, &serial_attr);
    return true;
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
