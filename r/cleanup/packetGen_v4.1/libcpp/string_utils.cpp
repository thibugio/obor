/* string_utils.cpp */


#include <vector>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>


#include "string_utils.h"

using namespace std;


/* calculate 8-bit fletcher checksum using blocksize=4 */
std::string fletcher8String(int len, std::string data) {
    std::vector<uint8_t> bytes;
    for (int i=0; i<len; i++)
        bytes.push_back((unsigned char)data[i]);

    uint8_t chk = fletcher8(len, bytes.data());
   
    std::string checkStr(1, chk);
    return checkStr;
}


/* create a string representataion of an entire packet to send over serial */
std::string compactPacketString(std::vector<catheterChannelCmd>& cmdVect, int cmdIndex) {

     int ncmds = cmdVect.size();      
     std::string outputStr;

     outputStr.append(compactPreambleString(cmdIndex, ncmds))
              .append(compactCommandVectString(cmdVect))
              .append(compactPostambleString(cmdIndex));
     std::string chkstr = fletcher8String(PCK_LEN(1)-1, outputStr);
     outputStr.append(chkstr);
 
     return outputStr;
}

/* construct the Preamble string */
std::string compactPreambleString(int cmdIndex, int ncmds) {

    char b1 = 0;    /* first byte of the preamble */

    b1 |= (PCK_OK << 7);             /* ok1 */
    b1 |= (cmdIndex & 7) << 4;    /* index3 */
    b1 |= (ncmds & 15);           /* cmdCnt4 */
    
    std::string preStr(1, b1);
    return preStr;
}

/* construct the Postamble string */
std::string compactPostambleString(int cmdIndex) {
    
    char b1 = 0;

    b1 |= cmdIndex << 5;  // index3
    b1 |= PCK_OK & 1;     // packet OK bit appended to beginning and end of packet
    
    std::string postStr(1, b1);    
    return postStr;
}

/* construct the Commands string from multiple commands in command vector */
std::string compactCommandVectString(const std::vector<catheterChannelCmd>&commandsIn) {

    std::string outputString;
    int ncmds = commandsIn.size();

    for(int i = 0; i < ncmds; i++) {
        std::string tempString = compactCommandString(commandsIn[i]);
        outputString.append(tempString);
    }
    return outputString;
}

/* construct a string from a single command. used by compactCommandVectString. */
std::string compactCommandString(const catheterChannelCmd& inputCmd) {
    
    std::string outputStr;
    outputStr.resize(CMD_LEN);

    uint8_t bytes[CMD_LEN];
    bytes[0] = (inputCmd.addr4 << 4);          // bits 1-4
    bytes[0] |= (inputCmd.cmd4 & 15);          // bits 5-8
    bytes[1] = (inputCmd.data12 >> 4) & 255;   // bits 9-16  (first 8 bits of data)
    bytes[2] = (inputCmd.data12 & 15) << 4;    // bits 17-20 (last 4 bits of data)
    // bits 21-24 reserved
//    printf("cmd data and checksum before string: %d\n", bytes[2]);
    for(int i = 0; i <3; i++) {
        outputStr[i] = bytes[i];
    }
//    printf("cmd data and checksum after string: %d\n", outputStr[2]);
    return outputStr;
}


/* parse input string into set of commands from arduino */
std::vector<catheterChannelCmd> expandString(std::string inputString) {
    //output Vector:
    std::vector<catheterChannelCmd> results;
    results.clear();

    //read the input string:
    int inStringSize = inputString.size();

    //if the string is poorly sized, trash it.
    if(inStringSize % CMD_LEN != (PRE_LEN + POST_LEN + PCK_CHK_LEN)) {
        //printf("There are an invalid number of charactors in the the string : %d\n",n);
        return results;
    }

    //check the first and last bytes:
    unsigned char preAmble = inputString.c_str()[0];
    unsigned char postAmble = inputString.c_str()[inStringSize - 1];

    unsigned int validPre =(unsigned int) (preAmble >> 7);
    unsigned int validPost =(unsigned int) postAmble & 1;
    if (!(validPre && validPost)) {
        return results;
    }

    unsigned int cmdIndexPre = (unsigned int) (preAmble >> 4) & 7;
    unsigned int cmdIndexPost = (unsigned int) (postAmble >> 5);
    if (cmdIndexPre != cmdIndexPost) {
        return results;
    }

    int cmdCountPre  = (int) (preAmble) & 15;

    //printf("The command is valid: %d and %d\n",validPre,validPost);
    //printf("The command index is: %d and  %d\n",cmdIndexPre,cmdIndexPost);
    //printf("The number of commands  is: %d\n",cmdCountPre);
	
    if(PCK_LEN(cmdCountPre) != inStringSize) {
        //printf("The packet is misaligned \n");
        return results;
    }

    for(int i = 0; (i*CMD_LEN + PRE_LEN + POST_LEN + PCK_CHK_LEN + 1) < inStringSize; i++) {
        catheterChannelCmd newResult;

        //load in the new charactors.
        unsigned char c1 = inputString.c_str()[i*CMD_LEN+1];
        unsigned char c2 = inputString.c_str()[i*CMD_LEN+2];
        unsigned char c3 = inputString.c_str()[i*CMD_LEN+3];

        //Process the new command charactors:
        newResult.addr4 = (unsigned int)  c1 >> 4;
        newResult.cmd4 = compactCmdVal((c1>>3)&1, (c1>>2)&1, (c1>>1)&1, c1&1);
        newResult.data12 = (unsigned int) ((c2 << 8) | (c3 >> 4)); 
        //newResult.chksum4 = (unsigned int) c3 & 15; 

        results.push_back(newResult);
    }

    //finished processing return the results.
    return results;
}

std::string compactResetCmd(int cmdIndex) {

    catheterChannelCmd reset;
    
    reset.addr4 = 0;
    reset.cmd4 = compactCmdVal(0,0,1,0);    // poll, en, update, dir
    reset.data12 = 0;

    std::vector<catheterChannelCmd> resetV(1, reset);

    return compactPacketString(resetV, cmdIndex);
}


/* verify that an incoming string is valid */
int validateStringIn(const std::string& stringIn,int &cmdIndex,int &cmdCount) {

    int valid = 0;

    //analyze preamble byte:
    unsigned char preAmble = stringIn.c_str()[0];
    unsigned int validPre =(unsigned int) (preAmble >> 7);      //packet OK bit
    unsigned int cmdIndexPre = (unsigned int) (preAmble >> 4) & 7;
    unsigned int commandCount = (unsigned int) (preAmble & 15);

    valid += (int) validPre;

    //if the string is poorly sized, trash it.
    if(stringIn.size()  <  PCK_LEN(commandCount)) {
       valid += 0;
       cmdIndex=cmdIndexPre;
       return valid;
    } else {
        valid += 2;
    } 

    //analyze postamble byte:
    unsigned char postAmble = stringIn.c_str()[PCK_LEN(commandCount) - 1];
    unsigned int validPost =(unsigned int) (postAmble & 1);

    valid += (int) validPost<<2;

    unsigned int cmdIndexPost = (unsigned int) (postAmble >> 5);
    cmdIndex = cmdIndexPre;
    if(cmdIndexPost == cmdIndexPre) {
        valid += 8;
    }
    
    return valid;
}

std::vector<std::string> getSingleCmdPacketStrings(std::vector<catheterChannelCmd>& commandVect) {
    int ncmds = commandVect.size();
    int cmdIndex = 1;

    std::vector<std::string> pstrings;
    for(int i = 0; i < ncmds; i++) {
        std::vector<catheterChannelCmd> singleCmdVect(1, commandVect[i]);
        pstrings.push_back(compactPacketString(singleCmdVect, cmdIndex));
        cmdIndex = (cmdIndex & 7) + 1;
    }
    return pstrings;
}

void getPacketStrings(std::vector<catheterChannelCmd>& commandVect, std::vector<std::string>& pstrings,
                      std::vector<int>& pdelays) {
    pstrings.clear();
    pdelays.clear();
    
    int ncmds = commandVect.size(); 
    int cmdIndex = 1; //README: cmdIndex is really a packetIndex
    
    std::vector<catheterChannelCmd> tempV;
    
    for(int i = 0; i < ncmds; i++) {
        tempV.push_back(commandVect[i]);
        if (commandVect[i].waitTime > 0 || i==(ncmds-1)) {
            pstrings.push_back(compactPacketString(tempV, cmdIndex));
            pdelays.push_back(commandVect[i].waitTime);
            tempV.clear();
            cmdIndex = (cmdIndex & 7) + 1;
        }        
    }
}


