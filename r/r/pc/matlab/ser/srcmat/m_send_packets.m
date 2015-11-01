function m_send_packets(npackets, packets, waits, lengths, port)
    CMDLEN=3;
    
    ncmds = zeros(1,npackets);
    for i=1:npackets
        ncmds(1,i) = (lengths(1,i)-3)/CMDLEN;
    end
    
    pause_ms = 100; 

    for i=1:npackets
        fwrite(port, packets(1:lengths(1,i), i), 'uint8');
        pause_cnt_ms = 0;
        while(port.BytesAvailable == 0)             
            pause(pause_ms/1000);
            pause_cnt_ms = pause_cnt_ms + pause_ms;
            fprintf('\nwaiting for port, paused %dms\n',pause_cnt_ms);
        end
        bytes = port.BytesAvailable;
        ret = fread(port, bytes);
        pckIndex = bitand(bitshift(ret(1), -4),7);
        fprintf('received %d bytes for packet index %d:\n', bytes, pckIndex);
        for j=1:bytes
            fprintf('%d ',ret(j));
        end
        fprintf('\n');
        switch bytes
            case 1
                fprintf('error in transmission: ');
                fprintf('error code = %d\n', ret-bitshift(pckIndex,4));
            case 2
                fprintf('error in transmission: ');
                fprintf('error code = %d\n', ret(2)-bitshift(pckIndex,4));
            case 2*ncmds(1,i) + 2
                fprintf('transmission successful\n');
            otherwise
                fprintf('not all commands executed\n');
        end
        new_wait_ms = waits(1,i) - pause_cnt_ms;
        if (new_wait_ms < 0)
          new_wait_ms = 0;
        end
        pause(new_wait_ms/1000);
    end
end