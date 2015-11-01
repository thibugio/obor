function m_preview_packets(npackets, packets, waits, lengths)
    CMDLEN=3;
    ncmds = zeros(size(lengths));
    str = '';
    for i=1:npackets
        fprintf('packet %d (post-delay=%dms):\n', i,waits(1,i));
        ncmds(1,i) = (lengths(1,i)-3)/CMDLEN;
        binary = dec2bin(uint8(packets(1:lengths(1,i), i)), 8);  
        pcklen=lengths(1,i);
        for j=1:pcklen
            tempStr = sprintf('%d, ',packets(j,i));
            fprintf('%s',tempStr);
            str = strcat(str, tempStr);
        end
        fprintf('\n');
        str = strcat(str, '\n');
    end
    str
    d = dialog('Name', 'Packet Preview');
    uicontrol('Parent', d, 'Style', 'text', 'String', str, 'Units', 'normalized',...
        'Position', [0,0,1,1]);
    
end