/* common_utils.h
*/

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <vector>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>


/* number of channels being used */
#define NCHANNELS 6 

/* number of bytes in a channel command, preamble, postamble, and packet checksum */
#define CMD_LEN 3  
#define PRE_LEN 1
#define POST_LEN 1
#define PCK_CHK_LEN 1

/* error codes for arduino to send back to PC */
#define PRE_ERR 1
#define POST_ERR 2
#define PCK_CHK_ERR 4

/* macro to compute the size of a packet */
#define PCK_LEN(N_CMDS) PRE_LEN + (CMD_LEN*N_CMDS) + POST_LEN + PCK_CHK_LEN
#define NCMDS(PCKLEN) (PCKLEN-PRE_LEN-POST_LEN-PCK_CHK_LEN)/CMD_LEN

#define PCK_OK 1

#define DAC_RES 4096
#define MAX_CURRENT_MA 1000

/* command value bits */
#define DIR_B 0
#define UPD_B 1
#define ENA_B 2
#define POL_B 3

#define ON 1
#define OFF 0


enum dir_t {
    dir_pos,
    dir_neg,
    dir_nochange
};


/* a command sent to a channel on the arduino */
struct catheterChannelCmd {
    unsigned int addr4;     /* channel address: 4 bits */
    unsigned int cmd4;      /* channel command number: 4 bits (poll, enable, update, direction) */
    uint8_t poll;
    uint8_t ena;
    uint8_t upd;
    dir_t dir;
    unsigned int data12;    /* data sent/received to/from the channel dac or adc: 12 bits */
    unsigned long waitTime; /* time length of the delay (not actually sent to arduino) */

};

struct strictCatheterPacket {
    unsigned int pseqnum;
    unsigned int delay;
    struct catheterChannelCmd cmds[NCHANNELS];
};

/* a serial packet to be sent to the arduino */
struct catheterPacket {
    unsigned int pseqnum;
    unsigned int delay;
    std::vector<catheterChannelCmd> cmds;
};


/** \brief uint8_t compactCmdVal(int poll, int en, int update, int dir): return the command number (0-15) associated with the specified state */
uint8_t compactCmdVal(int poll, int en, int update, int dir);

/** \brief void expandCmdVal(uint8_t cmdVal, int * poll, int * en, int * update, int * dir): return the state associated with the specified commnd number (0-15) */
void expandCmdVal(uint8_t cmdVal, int * poll, int * en, int * update, int * dir);

/** \brief uint8_t fletcher8(int len, uint8_t bytes[]): compute the fletcher checksum of an array of bytes of length 'len' using blocksize=8. ('len' <= the actual length of the array, since we may not want to include all elements of the array in the computation.) */
uint8_t fletcher8(int len, uint8_t bytes[]);

/** \brief std::vector<uint8_t> compactPacketBytes(std::vector<catheterChannelCmd>&,int): compacts a packet into a string to be sent over serial */
std::vector<uint8_t> compactPacketBytes(std::vector<catheterChannelCmd>&,int);

/** \brief std::vector<uint8_t> compactPreambleBytes(int cmdIndex,int cmdCount): uses the command index and number of commands to generate the preamble bit(s). */
std::vector<uint8_t> compactPreambleBytes(int cmdIndex,int cmdCount);

/** \brief std::vector<uint8_t> compactPostamble(int cmdIndex): uses the command index to generate the postamble bit(s). */
std::vector<uint8_t> compactPostambleBytes(int cmdIndex);

/** \brief std::vector<uint8_t> compactCommandVectBytes(const std::vector<catheterChannelCmd>&): generalize version of generating a command for multiple channels simultaneously. */
std::vector<uint8_t> compactCommandVectBytes(const std::vector<catheterChannelCmd>&);

/** \brief std::vector<uint8_t> compactCommandBytes(const catheterChannelCmd): compacts a single arduino command into a 3 byte packet. */
std::vector<uint8_t> compactCommandBytes(const catheterChannelCmd&);

void getPacketBytes(std::vector<catheterChannelCmd>& commandVect, std::vector<std::vector<uint8_t>>& pbytes, 
                    std::vector<int>& pdelays);

void convertCurrent(double current, unsigned int * res, dir_t * dir);

#endif
