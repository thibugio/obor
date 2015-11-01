/* mexcpp/mexPacketBytesFromReset.cpp */

#include "common_utils.h"
#include "pc_utils.h"
#include "mex_utils.h"


using namespace std;

/* for matlab-- populate a vector of packet bytes and a vector of inter-packet wait-times */
void mexPacketBytesFromReset(std::vector<std::vector<uint8_t>>& packetVect, std::vector<int>& waits) {
    packetVect.clear();
    waits.clear();    

    std::vector<catheterChannelCmd> commandVect; 
    commandVect.push_back(resetCommand());
    
    int ncmds = commandVect.size(); 
    int cmdIndex = 1; //README: cmdIndex is really a packetIndex
    
    //std::vector<catheterChannelCmd> tempV;    
    for(int i = 0; i < ncmds; i++) {
        mexPrintf("command %d:\n", i);
        mexSummarizeCmd(commandVect[i]);                
    }
    getPacketBytes(commandVect, packetVect, waits);
}

/* plhs[]: array of output values
 * prhs[]: array of input values
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
  
  /* Check for proper number of arguments */
  if (nrhs > 0) {
    mexErrMsgIdAndTxt("MATLAB:mexcpp:nargin", 
            "getPacketStringsFromPlayfile requires 0 input arguments.");
  } else if (nlhs != 3) {
    mexErrMsgIdAndTxt("MATLAB:mexcpp:nargout",
            "getPacketStringsFromPlayfile requires 3 output arguments.");
  }

  std::vector<int> lengths;
  std::vector<int> waits;
  std::vector<std::vector<uint8_t>> packetVect;
  mexPacketBytesFromReset(packetVect, waits);
  
  int npackets = packetVect.size();
  int maxPacketLen = PCK_LEN(1); 
  for (int i=0; i<npackets; i++) {
      lengths.push_back((int)(packetVect[i].size()));
      if (packetVect[i].size() > maxPacketLen) {
          maxPacketLen = packetVect[i].size();
      }
  }
  
  mwSize dims[2];
  dims[0] = maxPacketLen; //each row is an element of a packet
  dims[1] = npackets;  //each column is a packet
  plhs[0] = mxCreateNumericArray(2, dims, mxUINT8_CLASS, mxREAL);
         
  unsigned char * start_of_pr = (unsigned char *)mxGetData(plhs[0]);
  size_t bytes_to_copy = dims[0] * mxGetElementSize(plhs[0]);
  for (int i=0; i<npackets; i++) {
      size_t packet_bytes = packetVect[i].size() * mxGetElementSize(plhs[0]);
      memcpy(start_of_pr, packetVect[i].data(), packet_bytes);
      start_of_pr += bytes_to_copy;
  }
  
  
  dims[0] = 1; //each row is a wait time
  dims[1] = npackets;  //each column is a packet
  plhs[1] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);
  mexCopy2DIntArray(waits.data(), plhs[1], 1, npackets);
  
  dims[0] = 1; //each row is a packet length
  dims[1] = npackets;  //each column is a packet
  plhs[2] = mxCreateNumericArray(2, dims, mxUINT32_CLASS, mxREAL);
  mexCopy2DIntArray(lengths.data(), plhs[2], 1, npackets);
  
  return;
}

