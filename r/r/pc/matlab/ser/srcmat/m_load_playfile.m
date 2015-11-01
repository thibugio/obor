function [npackets, packets, delays, pckLens, channelCurrents] = m_load_playfile(path, maxPackets)
    NCHANNELS=6;
    CMDLEN=3;
    PRELEN=1;
    POSTLEN=1;
    CHKLEN=1;
    maxPckLen = CMDLEN*NCHANNELS + PRELEN + POSTLEN + CHKLEN;
    
    npackets = 0;
    packets = zeros(maxPckLen, maxPackets);
    delays = zeros(1,maxPackets);
    pckLens = zeros(1,maxPackets);    
    channelCurrents = zeros(1,maxPackets);
    
    [p, w, l] = mexPacketBytesFromPlayfile_wArgs(path);
    temp = size(p);
    npackets = temp(2);
    
    delays(1:npackets) = w;
    pckLens(1:npackets) = l;
    
    for i=1:npackets
        packets(1:pckLens(1,i), i) = p(1:pckLens(1,i),i);
        p1 = uint32(packets(3,i));
        p2 = uint32(packets(4,i));
        data = p1 * 2^4 + bitshift(p2, -4);
        channelCurrents(1,i) = data;
    end
end