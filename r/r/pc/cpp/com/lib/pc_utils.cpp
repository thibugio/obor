/* 
 * pc_utils.cpp
 */

//search README

#include "pc_utils.h"


using namespace std;


/* parse a playfile into a command vector */
int loadPlayFile(const char* fileIn, std::vector<catheterChannelCmd>& outputCmdsVect) {

    ifstream inFile(fileIn,ifstream::in);
    if(inFile.bad()) return -1;
	
    outputCmdsVect.clear();
    int npackets = 0;

    string line;
    int linenum = 0;
    while (getline (inFile, line)) {
		
        string item;

        size_t posOcto1  = line.find ("#");
        size_t posComma1 = line.find (",");
        size_t posComma2 = line.find (",",posComma1+1);
        size_t posComma3 = line.find (",",posComma2+1);

        //verify the line's validity.
        if(posComma1 < posComma2 && posComma2 < posComma3 && posComma3 < posOcto1) {  //line is ok.

            istringstream linestream(line);
        
            catheterChannelCmd cmd;
       
            /* parse channel */
            getline (linestream, item, ','); 
            int channelIn = atoi(item.c_str());
            if(!(channelIn >= 0 && channelIn <= NCHANNELS)) continue;   // bad channel; skip line
            cmd.addr4 = channelIn;

            /* parse DAC data */
            getline (linestream, item, ',');
            int dacData = atoi(item.c_str());
            if(!(dacData >= 0 && dacData < DAC_RES)) continue;    // bad data; skip line
            cmd.data12 = dacData;

            /* parse direction */
            getline (linestream, item, ',');
            int direction = atoi(item.c_str());
            int dir = direction;
            if (direction < 0) {
                if (linenum < 1) 
                    continue;   // bad direction
                else 
                    dir = (outputCmdsVect[linenum-1].cmd4 << DIR_B) & 1;
            }

            /* create command number */
            int en = dacData;
            int update = 0;
            int poll = 1;
            if (en || (linenum > 0 && ((unsigned int)dir != ((outputCmdsVect[linenum-1].cmd4 << DIR_B) & 1)))) {
                update = 1;
                poll = 0;
            }
            cmd.cmd4 = compactCmdVal(poll, en, update, dir);

            /* parse delay */
            getline (linestream, item, '#'); //works even if there is no #.
            int waitTime = atoi(item.c_str());
            if(!(waitTime >= 0)) continue;  // bad delay; skip line
            cmd.waitTime = waitTime;
            
            outputCmdsVect.push_back(cmd);
            
            if (waitTime) {
                npackets++;
            }
            linenum++;
        }
    }
    return npackets;
}


void summarizeCmd(const catheterChannelCmd& cmd) {
    printf("channel: %d\n",cmd.addr4);
    printf("poll: %d\n", (cmd.cmd4 >> POL_B) & 1);
    printf("enable: %d\n", (cmd.cmd4 >> ENA_B) & 1);
    printf("update: %d\n", (cmd.cmd4 >> UPD_B) & 1);
    printf("dir: %d\n", (cmd.cmd4 >> DIR_B) & 1);
    printf("data: %d\n", cmd.data12);
}

void print_string_as_bits(int len, std::string bytes) {
    int i,j;
    for (i=0; i<len; i++) {
        for (j=7; j>=0; j--) {
            if ((bytes[i] & (1<<j)))  printf("1");
            else  printf("0");
        }
        printf(" ");
    }
    printf("\n");
}

void print_bytes_as_bits(int len, std::vector<uint8_t> bytes) {
    int i,j;
    for (i=0; i<len; i++) {
        for (j=7; j>=0; j--) {
            if ((bytes[i] & (1<<j)))  printf("1");
            else  printf("0");
        }
        printf(" ");
    }
    printf("\n");
}

void getPacketBytesFromPlayfile(const char * path, std::vector<std::vector<uint8_t>>& pbytes, 
                                std::vector<int>& pdelays) {
    pbytes.clear();
    pdelays.clear();    

    std::vector<catheterChannelCmd> commandVect; 
    
    loadPlayFile(path, commandVect);
    
    getPacketBytesFromCommands(commandVect, pbytes, pdelays);
}
