path = 'C:/Users/acceber/Documents/MATLAB/cwru/research/cavusoglu/catheter/matlabPacketGen_v4.3/play/catheter.play';
maxPackets = 1;
[npackets, packets, delays, pckLens, channelCurrents] = m_load_playfile(path, maxPackets)