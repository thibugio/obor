/* file: srccpp/main.cpp */

#include "common_utils.h"
#include "pc_utils.h"
#include "SerialSender.h"

#include <cstring>
#include <cstdio>

#define PATH1 "/home/acceber/common/workspaces/matlab/cwru/research/cavusoglu/catheter/"
#define PATH2 "/home/acceber/common/cwru/research/Cavusoglu/catheter/code/pc/"

int main(int argc, char * argv[]) {

    SerialSender ss;
    
    std::string serialPath = ss.SerialSender::showSerialPath();
    printf("SerialSender detected device at %s.\n", serialPath.c_str());
    
    std::vector<std::vector<uint8_t>> packets;
    std::vector<int> packetDelays;

    std::string path(PATH2);
    path += "matlabPacketGen_v4.3.1/play/catheter.play";
    printf("reading playfile from %s\n", path.c_str());
    getPacketBytesFromPlayfile(path.c_str(), packets, packetDelays);

    ////print results////
    int npackets = packets.size();
    for (int i=0; i<npackets; i++) {
        printf("packet %d (delay=%dms):\n", i, packetDelays[i]);
        print_bytes_as_bits(packets[i].size(), packets[i]);
    }

    if(!(ss.connect(Baud::BR_9600)))
        printf("failed to open connection to port %s\n", serialPath.c_str());
    else
        printf("successfully opened connection to port %s\n", serialPath.c_str());

    // send the packets
    std::vector<std::vector<uint8_t>> bytesRead;
    int nbytes = ss.sendByteVectorsAndRead(packets, bytesRead, packetDelays);
    printf("read a total of %d bytes.\n", nbytes);
    for (int i=0; i<npackets; i++) {
        printf("bytes returned for packet %d: \t", i);
        print_bytes_as_bits(bytesRead[i].size(), bytesRead[i]);
    }

    ss.disconnect();

    return 0;
}
