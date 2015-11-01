/* 
 * pc_utils.h
 * defines methods, constants to be used only by the PC application
 */


#ifndef PC_UTILS_H
#define PC_UTILS_H

#include "common_utils.h"
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

enum filepath_t { 
    fp_playpath=0,
    fp_savepath,
    fp_logpath,
    fp_NFILES
};

struct files_t {
    const char * paths[fp_NFILES];
    std::fstream streams[fp_NFILES];
};

/** \brief int loadPlayFile(const char*,std::vector<catheterChannelCmd> &): Loads and parses a playfile.
 * The playfile is a recorded vector of computed commands with inter command delay. */
int loadPlayFile(const char * fname, std::vector<catheterChannelCmd>& cmdVect);

/** \brief void summarizeCmd(const catheterChannelCmd& cmd): summarizes a command reply.
 * This should not be used with gui's */
void summarizeCmd(const catheterChannelCmd& cmd);

void print_string_as_bits(int len, std::string s);

void print_bytes_as_bits(int len, std::vector<uint8_t> bytes);

#endif
