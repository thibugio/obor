function [npackets, packets, delays, pckLens, channelCurrents] = m_get_reset_packet(maxPackets)
    NCHANNELS=6;
    CMDLEN=3;
    PRELEN=1;
    POSTLEN=1;
    CHKLEN=1;
    maxPckLen = CMDLEN*NCHANNELS + PRELEN + POSTLEN + CHKLEN;
    
    npackets = 1;
    packets = zeros(maxPckLen, maxPackets);
    delays = zeros(1,maxPackets);
    pckLens = zeros(1,maxPackets);    
    channelCurrents = zeros(1,maxPackets);
    
    [p, w, l] = mexPacketBytesFromReset();
    
    delays(1:npackets) = w;
    pckLens(1:npackets) = l;
    

    packets(1:pckLens(1,1), 1) = p(1:pckLens(1,1),1);
    p1 = uint32(packets(3,1));
    p2 = uint32(packets(4,1));
    data = p1 * 2^4 + bitshift(p2, -4);
    channelCurrents(1,1) = data;

    
end