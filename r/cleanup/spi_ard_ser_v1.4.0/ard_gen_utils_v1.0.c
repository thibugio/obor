/* file: ard_gen_utils_v1.0.c
*/

#include <stdint.h>
#include <stdlib.h>


/* number of channels being used */ 
#define NCHANNELS 6 

/* number of bytes in a channel command, preamble, post-amble, packet checksum */
#define CMD_LEN 3  
#define PRE_LEN 1
#define POST_LEN 1
#define PCK_CHK_LEN 1

/* the expected length of a packet with a specified number of commands */
#define PCK_LEN(NCMDS) PRE_LEN + CMD_LEN*NCMDS + POST_LEN + PCK_CHK_LEN

#define PCK_OK 1

/* error codes for arduino to send back to PC */
#define PRE_ERR 1
#define POST_ERR 2
#define PCK_CHK_ERR 4

#define POL_B 3
#define ENA_B 2
#define UPD_B 1
#define DIR_B 0

/* structs to represent channel status, channel commands, and serial packets */

/* book-keeping for each channel */
struct channelStatus {
  int enable;
  int dir;
  uint16_t DAC_val;
  uint16_t ADC_val;
};

/* brief representation of a channel command */
struct channelCmd {
    uint8_t addr4;
    uint8_t cmd4;
    uint16_t data12;
    //uint8_t chksum4;
};

/* brief representation of a serial packet */
struct serialPacket {
    uint8_t ok1;
    uint8_t index3;
    uint8_t cmdCnt4;
    uint8_t reserved5;
    //uint8_t chksum8;
    struct channelCmd cmds[];
};


/* methods */

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

