#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <math.h>

#include "test_utils.h"

int decode_le(std::vector<uint8_t> data) {
    int val=0;
    //msb at highest index
    for (int i=0; i<(int)data.size(); i++) {
        val += data[i] * (int)(pow(2, i));
    }
    return val;
}

int decode_be(std::vector<uint8_t> data) {
    int val=0;
    //msb at lowest index
    for (int i=0; i<(int)data.size(); i++) {
        val += data[i] * (int)(pow(2, data.size() - i - 1));
    }
    return val;
}


void assertfail(bool cond, const char * msg) {
    if (!cond) {
        printf("%s\n", msg);
        exit(EXIT_FAILURE);
    }
}

void asserterr(bool cond, const char * msg) {
    if (!cond) {
        printf("%s\n", msg);
    }
}
