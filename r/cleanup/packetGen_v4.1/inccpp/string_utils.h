/* string_utils.h */

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "common_utils.h"


/** \brief std::string compactPacketString(std::vector<catheterChannelCmd>&,int): compacts a packet into a string to be sent over serial */
std::string compactPacketString(std::vector<catheterChannelCmd>&,int);

/** \brief std::string compactPreambleString(int cmdIndex,int cmdCount): uses the command index and number of commands to generate the preamble bit(s). */
std::string compactPreambleString(int cmdIndex,int cmdCount);

/** \brief std::string compactPostamble(int cmdIndex): uses the command index to generate the postamble bit(s). */
std::string compactPostambleString(int cmdIndex);

/** \brief std::string compactCommandVectString(const std::vector<catheterChannelCmd>&): generalize version of generating a command for multiple channels simultaneously. */
std::string compactCommandVectString(const std::vector<catheterChannelCmd>&);

/** \brief std::string compactCommandString(const catheterChannelCmd): compacts a single arduino command into a 3 byte packet. */
std::string compactCommandString(const catheterChannelCmd&);

/** \brief std::vector<catheterChannelCmd> expandString(std::string inputStr): parses an input string into a set of commands from the arduino. */
std::vector<catheterChannelCmd> expandString(std::string inputStr);

/** \brief int validateStringIn(const std::string&,int &cmdIndex): Verifies that the incoming string is valid.
 * Also determines the associated command index. */
int validateStringIn(const std::string&,int &cmdIndex, int &cmdCount);

/** \brief std::string compactResetCmd(int cmdIndex): generates a global reset command to the arduino. */
std::string compactResetCmd(int cmdIndex);

/** \brief std::string fletcher8String(int len, std::string bytes): compute the fletcher checksum of an array of bytes of length 'len' using blocksize=8. */
std::string fletcher8String(int len, std::string data);

std::vector<std::string> getSingleCmdPacketStrings(std::vector<catheterChannelCmd>& commandVect);

void getPacketStrings(std::vector<catheterChannelCmd>& commandVect, std::vector<std::string>& pstrings,
                      std::vector<int>& pdelays);

#endif
