#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>

int decode_le(std::vector<uint8_t> data);

int decode_be(std::vector<uint8_t> data);

void assertfail(bool cond, const char * msg);

void asserterr(bool cond, const char * msg);

#endif
