/* common_utils.cpp */

#include "common_utils.h"

using namespace std;


/* return the (4 bit) command value associated with a particular state */
uint8_t compactCmdVal(int poll, int en, int update, int dir) {
    unsigned int cmd = 0;   
    if (poll)   cmd |= (1 << POL_B);
    if (en)     cmd |= (1 << ENA_B);
    if (update) cmd |= (1 << UPD_B);
    if (dir)    cmd |= (1 << DIR_B);
    return cmd;
}

/* return the state associated with a particular (4 bit) command value */
void expandCmdVal(uint8_t cmdVal, int * poll, int * en, int * update, int * dir) {
    *poll = (cmdVal >> POL_B) & 1;
    *en = (cmdVal >> ENA_B) & 1;
    *update = (cmdVal >> UPD_B) & 1;
    *dir = (cmdVal >> DIR_B) & 1;
}

/* calculate 8-bit fletcher checksum using blocksize=4 */
uint8_t fletcher8(int len, uint8_t data[]) {
    uint8_t sum1 = 0, sum2 = 0;
    int i;
    for (i=0; i<len; i++) {     
        sum1 += (data[i] >> 4);   //first 4 bits
        sum2 += sum1;
     
        sum1 += (data[i] & 15);   //last 4 bits
        sum2 += sum1;
     
        sum1 %= 15;   //modulo 15
        sum2 %= 15;
    }
    sum1 %= 15;
    sum2 %= 15;

    return ((sum2 & 15)<<4) + (sum1 & 15);
}

std::vector<uint8_t> compactPreambleBytes(int pseqnum, int ncmds) {
    std::vector<uint8_t> bytes;
    int i;
    for (i=0; i<PRE_LEN; i++) {
        if (i==0) {
            bytes.push_back(PCK_OK << 7);          /* ok1 */
            bytes[i] |= (pseqnum & 7) << 4;    /* index3 */
            bytes[i] |= (ncmds & 15);           /* cmdCnt4 */
        }
    }
    return bytes;
}

std::vector<uint8_t> compactCommandBytes(catheterChannelCmd& cmd) {
    std::vector<uint8_t> bytes;
    int i;
    for (i=0; i<CMD_LEN; i++) {
        if (i==0) {
            bytes.push_back(cmd.addr4 << 4);          // bits 1-4
            bytes[i] |= (cmd.cmd4 & 15);          // bits 5-8
        } else if (i==1) {
            bytes.push_back((cmd.data12 >> 4) & 255);   // bits 9-16  (first 8 bits of data)
        } else if (i==2) {
            bytes.push_back((cmd.data12 & 15) << 4);    // bits 17-20 (last 4 bits of data)
        }  // last 4 bits reserved
    }
    return bytes;
}

std::vector<uint8_t> compactCommandVectBytes(int ncmds, std::vector<catheterChannelCmd>& cmdVect) {
    std::vector<uint8_t> bytes;
    int i;
    for (i=0; i<ncmds; i++) {
        std::vector<uint8_t> tempV = compactCommandBytes(cmdVect[i]);
        bytes.insert(bytes.end(), tempV.begin(), tempV.end()); 
    }
    return bytes;
}

std::vector<uint8_t> compactPostambleBytes(int pseqnum) {
    std::vector<uint8_t> bytes;
    int i;
    for (i=0; i<POST_LEN; i++) {
        if (i==0) {
            bytes.push_back(pseqnum << 5);  // index3
            bytes[i] |= PCK_OK & 1;     // packet OK bit appended to beginning and end of packet
        }
    }
    return bytes;
} 

std::vector<uint8_t> compactPacketBytes(std::vector<catheterChannelCmd>& cmdVect, int pseqnum) {
    std::vector<uint8_t> bytes;
    int ncmds = cmdVect.size();
    
    std::vector<uint8_t> preV = compactPreambleBytes(pseqnum, ncmds);
    std::vector<uint8_t> cmdV = compactCommandVectBytes(ncmds, cmdVect);
    std::vector<uint8_t> postV = compactPostambleBytes(pseqnum);

    bytes.insert(bytes.end(), preV.begin(), preV.end());
    bytes.insert(bytes.end(), cmdV.begin(), cmdV.end());
    bytes.insert(bytes.end(), postV.begin(), postV.end());
    
    uint8_t chksum = fletcher8(PCK_LEN(ncmds)-1, bytes.data());
    bytes.insert(bytes.end(), 1, chksum);
    
    return bytes;
}

catheterChannelCmd resetCommand() {
    catheterChannelCmd cmd;
    cmd.addr4 = 0; //global
    cmd.data12 = 0;
    cmd.cmd4 = compactCmdVal(0, 0, 1, 0); // poll, ena, upd, dir
    cmd.waitTime = 3000; // 3 seconds...
    return cmd;
}

void getPacketBytesFromCommands(std::vector<catheterChannelCmd>& commandVect, 
                                std::vector<std::vector<uint8_t>>& pbytes, 
                                std::vector<int>& pdelays) {
    pbytes.clear();
    pdelays.clear();
    
    int ncmds = commandVect.size(); 
    int pseqnum = 1;
    
    std::vector<catheterChannelCmd> tempV;
    
    for(int i = 0; i < ncmds; i++) {
        tempV.push_back(commandVect[i]);
        if (commandVect[i].waitTime > 0 || i==(ncmds-1)) {
            pbytes.push_back(compactPacketBytes(tempV, pseqnum));
            pdelays.push_back(commandVect[i].waitTime);
            tempV.clear();
            pseqnum = (pseqnum & 7) + 1;
        }        
    }
}

//template <typename T>
uint8_t * serialize(std::vector<std::vector<uint8_t>>  bytevecs, size_t *n) {
    *n = 0;
    for (int i=0; i<(int)bytevecs.size(); i++) {
        *n = *n + bytevecs[i].size();
    }

    uint8_t * bytearray = (uint8_t *) malloc(*n * sizeof(uint8_t));

    *n = 0;
    for (int i=0; i<(int)bytevecs.size(); i++) {
        memcpy(bytearray + *n, bytevecs[i].data(), bytevecs[i].size());
        *n = *n + bytevecs[i].size();
    }
    return bytearray;
}
