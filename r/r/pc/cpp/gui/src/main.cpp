/* file: srccpp/main.cpp */

#include "common_utils.h"
#include "pc_utils.h"
#include "string_utils.h"

#include <cstring>
#include <cstdio>

#define PLAYPATH "/home/acceber/common/workspaces/matlab/cwru/research/cavusoglu/catheter/"\
                    "matlabPacketGen_v3.3/play/catheter.play"


int main(int argc, char * argv[]) {
    srand((unsigned int)time(NULL));

    //parse playfile into command vector
    std::vector<catheterChannelCmd> commandVect; 
    loadPlayFile(PLAYPATH, commandVect);
    
    int ncmds = commandVect.size();
    int cmdIndex = rand() & 7;
 
    //pack commands into single packet string
    std::string packetString = compactPacketString(commandVect, cmdIndex);

    //pack each command into its own packet string
    std::vector<std::string> singleCmdPacketStrings = getSingleCmdPacketStrings(commandVect);

    //pack commands into packets based on delays
    std::vector<std::string> pstrings;
    std::vector<int> pdelays;
    getPacketStrings(commandVect, pstrings, pdelays);

    //pack commands into byte-packets based on delays
    std::vector<std::vector<uint8_t>> pbytes;
    getPacketBytes(commandVect, pbytes, pdelays);


    ////print results////

    printf("command Index: %d\n", cmdIndex);
    printf("num commands: %d\n", ncmds);
    for (int i=0; i<ncmds; i++) {
        printf("\n");
        printf("command %d:\n", i);
        summarizeCmd(commandVect[i]);
        printf("command wait time: %d\n", (int)commandVect[i].waitTime);
    }  
    printf("\n----------------\n");

    printf("packet string (all commands):\n");
    for (int i=0; i<PCK_LEN(ncmds); i++) {
        print_string_as_bits(1, &packetString[i]);
    }
    printf("\n----------------\n");

    printf("\npacket strings (individual commands):\n\n");
    for (int i=0; i<ncmds; i++) {
        printf("packet %d:\n", i);
        print_string_as_bits(PCK_LEN(1), singleCmdPacketStrings[i]);
    }
    printf("\n----------------\n");

    int npackets = pstrings.size();
    for (int i=0; i<npackets; i++) {
        printf("packet %d (delay=%dms):\n", i, pdelays[i]);
        print_string_as_bits(pstrings[i].size(), pstrings[i]);
    }
    printf("\n----------------\n");

    npackets = pbytes.size();
    for (int i=0; i<npackets; i++) {
        printf("packet %d (delay=%dms):\n", i, pdelays[i]);
        print_bytes_as_bits(pstrings[i].size(), pbytes[i]);
    }


    return 0;
}
