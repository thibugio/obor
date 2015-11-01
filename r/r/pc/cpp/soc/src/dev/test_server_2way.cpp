#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <string>

#include "udp_server.h"
#include "test_utils.h"
#include "common_utils.h"
#include "pc_utils.h"

#define debug_ 1

//mmm.... global variables...
std::vector<CatheterPacket> packets;
std::vector<std::vector<uint8_t>> byteVecs;
uint8_t pnum;
uint8_t chan;
bool flag_global;
bool flag_return;

const char * byteVec2cstr(std::vector<uint8_t> bytes) {
    std::string str;
    for (int i=0; i<(int)bytes.size(); i++) {
        if (bytes[i]==0) continue;
        str.push_back((char)bytes[i]);
    }
    return str.c_str();
}

void sendCatheterPackets (std::vector<CatheterPacket> packets) {

}

bool isChannelIncluded (CatheterPacket p, uint8_t chan) {
    for (int i=0; i<(int)p.cmds.size(); i++) {
        if (p.cmds[i].addr4 == chan) 
            return true;
    }
    return false;
}

void new_packet() {
    struct CatheterPacket p;
    p.pseqnum = pnum;
    packets.push_back(p);
    if (debug_) printf("pushed new packet. pnum=%d, number of packets=%d\n", pnum, (int)packets.size());
}

void new_channel(int channel) {
    struct catheterChannelCmd c;
    c.addr4 = channel;
    c.pol = false;
    c.ena = false;
    c.upd = true;
    packets[pnum].cmds.push_back(c);
    if (debug_) printf("pushed new channel. pnum=%d, chan=%d, number of channels=%d\n", pnum, channel, (int)packets[pnum].cmds.size());
}

bool set_pnumber(std::vector<uint8_t> data) {
    int num = decode_be(data);
    if (num > (int)packets.size()) {
        if (debug_) printf("failed to set packet number=%d\n", num);
        return false;
    }
    pnum = num;
    if (debug_) printf("set packet number=%d\n", num);
    if (num == (int)packets.size()) 
        new_packet();
    return true;
}

void set_global() {
    if (packets[pnum].cmds.size() < NCHANNELS) {
        for (int i=0; i<NCHANNELS; i++) {
            if (!isChannelIncluded(packets[pnum], i)) {
                new_channel(i);
            }
        }
    }
    for (int i=0; i<NCHANNELS; i++) {
        if (debug_) printf("global: enabling channel %d\n", i);
        packets[pnum].cmds[i].ena = true; 
        packets[pnum].cmds[i].upd = true; 
    }
    flag_global = true;
}

bool set_channel(std::vector<uint8_t> data) {
    int num = decode_be(data);
    if (num <= NCHANNELS) {
        if (debug_) printf("set channel number=%d\n", num);
        if (num == 0) {
            flag_global = true;
            chan = num;
            set_global();
        } else {
            flag_global = false;
            chan = num - 1;
            if (!isChannelIncluded(packets[pnum], num)) 
                new_channel(chan);
        }
        return true;
    }
    if (debug_) printf("failed to set channel number=%d\n", num);
    return false;
}

void set_poll(std::vector<uint8_t> data) {
    bool poll = data[data.size()-1];
    for (int i=0; i<NCHANNELS; i++) {
        if (flag_global || i==chan) {
            packets[pnum].cmds[i].pol = poll;
            if (debug_) printf("set channel %d poll=%d\n", i, poll);
        }
    } 
}

void set_direction(dir_t dir) {
    for (int i=0; i<NCHANNELS; i++) {
        if (flag_global || i==chan) {
            packets[pnum].cmds[i].dir = dir;
            packets[pnum].cmds[i].upd = (packets[pnum].cmds[i].ena || 
              (packets.size() > 0 && (!isChannelIncluded(packets[pnum-1], i) ||
              packets[pnum-1].cmds[i].dir != dir)));
            if (debug_) printf("set channel %d direction and update.\n", i);
        }
    }
}

bool set_current(std::vector<uint8_t> data) {
    uint16_t res = (uint16_t)(decode_be(data));
    if (res >= DAC_RES) {
        if (debug_) printf("failed to set channel data=%d\n", res);
        return false;
    }
    bool ena = (res != OFF_RES);
    for (int i=0; i<NCHANNELS; i++) {
        if (flag_global || i==chan) {
            packets[pnum].cmds[i].data12 = res;
            packets[pnum].cmds[i].ena = ena;
            if (debug_) printf("set channel %d data=%d and enable=%d\n", i, res, ena);
        }
    }
    return true;
}

void set_delay(std::vector<uint8_t> data) {
    int delay = decode_be(data);
    packets[pnum].delay = delay;
    if (debug_) printf("set packet %d delay=%dms\n", pnum, delay);
}

void clear_packets() {
    packets.clear();
    pnum = 0;
    chan = 0;
    flag_global = false;
}

bool delete_packet(std::vector<uint8_t> data) {
    int num = decode_be(data);
    if (num >= (int)packets.size()) return false; 
    packets.erase(packets.begin()+num-1);
    if (pnum == num) {
       if (pnum > 0) pnum -= 1;
    }
    return true; 
}

bool delete_channel(std::vector<uint8_t> data) {
    int num = decode_be(data);
    if (num >= 0 && num < (int)packets[pnum].cmds.size()) {
        packets[pnum].cmds.erase(packets[pnum].cmds.begin() + num - 1);
        if (chan == num) {
            if (chan > 0) chan -= 1;
        }
        return true;
    }
    return false;
}

bool connect_serial() {
    return false;
} 

bool exit() {
    return true;
}

void send_packets() {
    sendCatheterPackets(packets);
}

void send_global_reset() {

}

void preview_packets() {
    printf("\nPACKET PREVIEW\n");
    for (int i=0; i<(int)packets.size(); i++) {
        printf("\nNEXT PACKET: seqnum=%d \tdelay=%dms\n", packets[i].pseqnum, (int)packets[i].delay); 
        for (int j=0; j<(int)packets[i].cmds.size(); j++) 
            summarizeCmd(packets[i].cmds[j]);
    }
}

void get_packet_bytes(std::vector<std::vector<uint8_t>> bytes) {
    bytes.clear();
    for (int i=0; i<(int)packets.size(); i++) {
        if (i>0)
            packets[i].pseqnum = (packets[i-1].pseqnum+1) % MAX_PSEQNUM;  //just in case
        bytes.push_back(compactPacketBytes(packets[i].cmds, packets[i].pseqnum));
    }
        
}

void preview_packet_bytes() {
    get_packet_bytes(byteVecs);
    if (debug_) {
        printf("\nPACKET BYTES\n\n");
        for (int i=0; i<(int)packets.size(); i++)
            print_bytes_as_bits(byteVecs[i].size(), byteVecs[i]);
    }
}

bool update(const char * field, std::vector<uint8_t> data) {
    if (!strcmp(field, "dlpkt")) {
        return delete_packet(data);
    } else if (!strcmp(field, "dlcha")) {
        return delete_channel(data);
    } else if (!strcmp(field, "pnum")) {
        return set_pnumber(data);
    } else if (!strcmp(field, "chan")) {
        return set_channel(data);
    } else if (!strcmp(field, "pol")) {
        set_poll(data);
    } else if (!strcmp(field, "dir")) {
        const char * dirStr = byteVec2cstr(data);
        if (!strcmp(dirStr, "pos"))
            set_direction(DIR_POS);
        else if (!strcmp(dirStr, "neg"))
            set_direction(DIR_NEG);
        else return false;
    } else if (!strcmp(field, "data")) {
        return set_current(data);
    } else if (!strcmp(field, "wait")) {
        set_delay(data);
    } else if (!strcmp(field, "send")) {
        flag_return = true;
        send_packets();
    } else if (!strcmp(field, "reset")) {
        send_global_reset();
    } else if (!strcmp(field, "glob")) {
        set_global();
    } else if (!strcmp(field, "conn")) {
        flag_return = true;
        return connect_serial();
    } else if (!strcmp(field, "exit")) {
        return exit();
    } else if (!strcmp(field, "clrpks")) {
        clear_packets();
    } else if (!strcmp(field, "prev")) {
        if (debug_) preview_packets();
        preview_packet_bytes();
        flag_return = true;
    } else { return false; }
    return true;
}


bool parse_packet (uint8_t bytes[], size_t nbytes) {
    std::string field;
    std::vector<uint8_t> data;
    bool sep=false;
    for (int i=0; i<(int)nbytes; i++) {
        if (bytes[i] == ' ') {
            sep=true;
            continue;
        }
        if (!sep) field.push_back((char)bytes[i]);
        else data.push_back(bytes[i]); 
    }
    if (debug_) printf("field: %s, data: %d\n", field.c_str(), decode_be(data));
    return update(field.c_str(), data);    
}

bool gate_function(uint8_t bytesIn[], size_t nbytesIn, uint8_t bytesOut[], size_t bytesOutSize, size_t *nbytesOut, bool *overflow) {
    bool ret = parse_packet(bytesIn, nbytesIn);
    if (ret) {
        if (flag_return) {        
            uint8_t * bytes_ret = serialize(byteVecs, nbytesOut);

            if (*nbytesOut <= bytesOutSize) {
                memcpy(bytesOut, bytes_ret, *nbytesOut);
                *overflow = false;
            } else {
                memcpy(bytesOut, bytes_ret, bytesOutSize);
                *overflow = true;
                *nbytesOut = bytesOutSize - *nbytesOut;
            }
            flag_return = false;
        } else {
            *nbytesOut = 0;
        }
    }
    return ret;
}

int main() {
    pnum=0;
    chan=0;
    packets.clear();
    flag_global = false;
    try {
        boost::asio::io_service io;
        udp_server server(io, gate_function);
        io.run();
    } catch (std::exception& e) {
        printf("ERROR: %s\n", e.what());
    }
    return 0;
}
