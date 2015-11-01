/* file: spi_and_ser_v1.4.ino */

#include "ard_gen_utils_v1.0.c"

#include <SPI.h>

#define DUE

/* *************** */
/* pin definitions */
/* *************** */

int DAC_CS_pins[NCHANNELS] = {2,3,4,5,6,7};
int ADC_CS_pins[NCHANNELS] = {8,9,10,11,12,13};
int H_Enable_pins[NCHANNELS] = {22,23,24,25,26,27};
int H_Neg_pins[NCHANNELS] = {34,35,36,37,38,39};
int H_Pos_pins[NCHANNELS] = {28,29,30,31,32,33};
int DAC_LDAC_pins[NCHANNELS] = {46,47,48,49,50,51};


#define INBUF_SIZE PCK_LEN(NCHANNELS)   // a reasonable upper bound on the maximum packet size.


/* **************** */
/* global variables */
/* **************** */

/* tracks status of channels */
channelStatus channelList[NCHANNELS];  
uint8_t packetStatus; // {X,X,X,X,PCK_CHK_ERR, POST_ERR, CMD_CHK_ERR, PRE_ERR} 
uint8_t packetIndex;
uint8_t ncmds;
uint8_t bytesRem;
uint8_t nextByte;
bool newPacket;
uint8_t packetBytes[PCK_LEN(NCHANNELS)];

bool noCmds;

/* *************** */
/* SPI definitions */
/* *************** */

#define START_DELAY 500     /* delay before first write to DAC */
#define DAC_ADC_DELAY 500   /* delay between write to DAC and read from ADC */
#define ADC_DAC_DELAY 500   /* delay between read from ADC and write to DAC */

#define DAC_SELECT  0b00000000    /* write to DAC A;          bit 15 */
#define INPUT_BUF   0b00000000    /* Vref input not buffered; bit 14 */
#define GAIN_SELECT 0b00100000    /* output gain = 1;         bit 13 */
#define PWR_DOWN    0b00010000    /* output power-down ctrl;  bit 12 */

/* ************* */
/* SPI functions */
/* ************* */

/* initialize the SPI bus */
void SPI_init() {
    SPI.begin();
    //SPI.setDataMode(SPI_MODE1); 
    #ifdef DUE
    SPI.setClockDivider(SPI_CLOCK_DIV4);    /* 20 MHz (due is 88, mega2560 is 16) */
    #endif
    SPI.setBitOrder(MSBFIRST);
}

/*  read a 12-bit value from the MCP3201 ADC on a specified channel (1 through NCHANNELS) */
uint16_t ADC_read(int channel) {
    digitalWrite(ADC_CS_pins[channel-1], LOW);
    byte b1 = SPI.transfer(0x00);
    byte b2 = SPI.transfer(0x00);      
    digitalWrite(ADC_CS_pins[channel-1], HIGH);   
    uint16_t ret = ((b1 << 8) | b2) >> 1;   //combine the bytes and get rid of blank first bit
    return ret;
}

/*  write a 12-bit value to the MCP4921 DAC on a specified channel (1 through NCHANNELS) */
void DAC_write(int channel, uint16_t to_dac) {
//    delay(100);
    digitalWrite(DAC_CS_pins[channel-1], LOW);
    byte dataMSB = highByte(to_dac);
    byte dataLSB = lowByte(to_dac);
//    byte dataMSB = highByte(2500);
//    byte dataLSB = lowByte(2500);
    dataMSB &= 0b00001111;
    dataMSB = dataMSB | DAC_SELECT | INPUT_BUF | GAIN_SELECT | PWR_DOWN;
    SPI.transfer(dataMSB);
    SPI.transfer(dataLSB);
    digitalWrite(DAC_CS_pins[channel-1], HIGH);
}


/* **************** */
/* Serial functions */
/* **************** */

/* start serial connection for Atmega2560 */
void serial_init() {
  #ifdef DUE
    //set millisecond timeout so commands can be entered
    SerialUSB.setTimeout(100); 
    SerialUSB.begin(9600);
    //while (! SerialUSB); // Wait untilSerial is ready - Leonardo
  #else
    Serial.setTimeout(100);
    Serial.begin(9600);
  #endif
}

int serial_available(void) {
  #ifdef DUE
    return SerialUSB.available();
  #else
    return Serial.available();
  #endif
}

void flush_bytes(int count) {
    int i;
    for (i=0; i<count; i++) {
    #ifdef DUE
      SerialUSB.read();
    #else
      Serial.read();
    #endif
  }
}

void write_byte(uint8_t b) {
  #ifdef DUE
    SerialUSB.write(b);
  #else
    Serial.write(b);
  #endif
}

void write_bytes(int nb, uint8_t bytes[]) {
  int i;
  for (i=0; i<nb; i++) {
    write_byte(bytes[i]);
  }
}

uint8_t read_byte(void) {
  #ifdef DUE
    return (uint8_t)(SerialUSB.read());
  #else
    return (uint8_t)(Serial.read());
  #endif  
}


/* ************************ */
/* initialization functions */
/* ************************ */

/* set pins to output mode and put them in defined state */
void pin_init() {
    for (int i=0; i<NCHANNELS; i++) {
        pinMode(ADC_CS_pins[i], OUTPUT);
        pinMode(DAC_CS_pins[i], OUTPUT);
        pinMode(H_Enable_pins[i], OUTPUT);
        pinMode(H_Neg_pins[i], OUTPUT);
        pinMode(H_Pos_pins[i], OUTPUT);
        pinMode(DAC_LDAC_pins[i], OUTPUT);
        
        digitalWrite(ADC_CS_pins[i], HIGH);
        digitalWrite(DAC_CS_pins[i], HIGH);
        digitalWrite(DAC_LDAC_pins[i], LOW);
        digitalWrite(H_Enable_pins[i], LOW);
        digitalWrite(H_Neg_pins[i], HIGH);
        digitalWrite(H_Pos_pins[i], HIGH);
    }
}
/* ***************** */
/* command functions */
/* ***************** */

/* enable or disable the H-bridge on a given channel (Active LOW) */
void toggle_enable(int channel, int en) {
    if (en == 0) {  // disable 
        digitalWrite(H_Enable_pins[channel-1], HIGH);
    } else {  // enable 
        digitalWrite(H_Enable_pins[channel-1], LOW);
    }
}

/* set the direction of the H-bridge for a given channel */
void set_direction(int channel, int direction) {
    if (direction == 0) {
        digitalWrite(H_Neg_pins[channel-1], HIGH);
        digitalWrite(H_Pos_pins[channel-1], LOW);
    } else {
        digitalWrite(H_Pos_pins[channel-1], HIGH);
        digitalWrite(H_Neg_pins[channel-1], LOW);
    }
}

/* execute a channel command */
uint16_t execute_channel_cmd(uint8_t cmdBytes[]) {
  uint16_t retVal = 0;
  
  uint8_t addr = cmdBytes[0] >> 4;
  uint8_t cmdVal = cmdBytes[0] & 15;
  uint16_t cmdData = (((int)(cmdBytes[1])) << 4) + (cmdBytes[2] >> 4);

  bool poll = (cmdVal >> 3) & 1;
  bool en   = (cmdVal >> 2) & 1;
  bool upd  = (cmdVal >> 1) & 1;
  bool dir  = (cmdVal >> 0) & 1;
  
  int start_addr = (addr > 0 ? addr : 1);  // addr 0 is global; address values 1-15 are valid.
  int stop_addr = (addr > 0 ? addr + 1 : NCHANNELS + 1);
    
    for (int i=start_addr; i<stop_addr; i++) {
        if (cmdVal == 0) continue; 

        if (poll) {  
            retVal = ADC_read(i);
            channelList[i].ADC_val = ADC_read(i);            
        } else {    
            toggle_enable(i, en);  /* enable or disable channel */
            channelList[i].enable = en;               
            if (upd) {  /* set DAC value */
                DAC_write(i, cmdData);
                channelList[i].DAC_val = cmdData;
            }     
            set_direction(i, dir);   /* set H-brige direction */
            channelList[i].dir = dir;
        }
    }
    return retVal;
}


/* ************ */
/* main program */
/* ************ */

void setup() {
    pin_init();
    SPI_init();
    serial_init();
    
    packetStatus = 0; // {X,X,X,X,PCK_CHK_ERR, POST_ERR, CMD_CHK_ERR, PRE_ERR} 
    packetIndex = 0;
    ncmds = 0;
    bytesRem = 0;
    nextByte = 0;
    newPacket = 1;

    noCmds = 1;

    for (int i=0; i<INBUF_SIZE; i++) {
      packetBytes[i] = 0;
    }
    
    delay(START_DELAY);
}

void loop() {
  if (noCmds) {
    for (int i=0; i<=NCHANNELS; i++) {
      DAC_write(i, (uint16_t)4095);
    }
  }
  while (serial_available() > 0) {
    nextByte = read_byte();
    if (newPacket) {
      //Serial.write(ncmds);  //OK
      newPacket = 0;
      packetBytes[0] = nextByte;      
      ncmds = nextByte & 15;
      bytesRem = PCK_LEN(ncmds)-1;
      packetIndex = (nextByte >> 4) & 7;
      if (nextByte < 128){
        packetStatus += PRE_ERR;
        flush_bytes(bytesRem-1);  // dont't want to completely clear buffer b/c won't ever reach 'else' block
      }
    } else {
      //Serial.write(count);
      packetBytes[PCK_LEN(ncmds)-bytesRem] = nextByte;
      bytesRem--;
      if (bytesRem == 0) {
        if (packetStatus==0) {
          if (packetIndex == ((packetBytes[PCK_LEN(ncmds)-PCK_CHK_LEN-1]) >> 5) & 7) {
            if (fletcher8(PCK_LEN(ncmds)-1, packetBytes) == packetBytes[PCK_LEN(ncmds)-1]) {
              noCmds = 0;
              write_byte(((int)(packetIndex) << 4) + (packetStatus & 15));  // initial packet Status byte                    
              for (int i=0; i<ncmds; i++) {
                uint8_t cmdBytes[CMD_LEN];
                for (int j=0; j<CMD_LEN; j++) {
                  cmdBytes[j] = packetBytes[PRE_LEN + i*CMD_LEN + j];
                }
                uint16_t cmdRet = execute_channel_cmd(cmdBytes);  // execute the channel command
                //cmdRet = (((int)(cmdBytes[1])) << 4) + (cmdBytes[2] >> 4);
                cmdRet = ((cmdBytes[0] & 15) >> 0) &1; 
                uint8_t cmdRetBytes[2] = {highByte(cmdRet), lowByte(cmdRet)};  // write 2 bytes for every command executed.
                write_bytes(2, cmdRetBytes);
              }
            } else {
              packetStatus += PCK_CHK_ERR;
            }
          } else {
            packetStatus += POST_ERR;
          }
        } //endif packetStatus==0
        write_byte(((int)(packetIndex) << 4) + (packetStatus & 15));  // final packet Status byte
        for (int i=0; i<INBUF_SIZE; i++) {
          packetBytes[i] = 0;
        }
        packetStatus = 0;
        newPacket = 1;
      }  //endif bytesRem==0
    }  //endif nextByte < 127
  }  //endwhile serial_available()
}  //end loop

