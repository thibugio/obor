/* test_serial_main.cpp */

#include "common_utils.h"
#include "CatheterSerialSender.h"

#include <cstdio>
#include <cstring>

#define PLAYPATH "/home/acceber/common/cwru/research/Cavusoglu/catheter/code/pc/"\
                    "matlabPacketGen_v4.3.1/play/catheter.play"
int main() {

    //parse playfile into command vector
    std::vector<catheterChannelCmd> commandVect; 
    loadPlayFile(PLAYPATH, commandVect);
    
    int ncmds = commandVect.size();

    //pack commands into byte-packets based on delays
    std::vector<std::vector<uint8_t>> pbytes;
    getPacketBytes(commandVect, pbytes, pdelays);

    CatheterSerialSender css();
    bool serialOK = css.init();
    if (!serialOK) {
        printf("CatheterSerialSender failed to initialize. Exiting program...\n"):
        exit(EXIT_FAILURE);
    }
    
    std::string path = css.getPath();
    printf("found device on path %s\n", path.c_str());

    css.sendByteVectors(pbytes, pdelays);
    std::vector<std::vector<uint8_t>> bytesRead = css.sendByteVectorsAndRead(pbytes, pdelays);
}
