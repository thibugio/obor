% file: matlab_serial.m

% includePath = 'C:\Users\acceber\Documents\MATLAB\cwru\research\cavusoglu\catheter\matlabPacketGen_v4.1\inccpp\';
% commonUtilsPath = strcat(includePath, 'common_utils.h');
% if (~libisloaded('common_utils'))
%     loadlibrary('common_utils', commonUtilsPath);
% end
% pause;

% connect serial port to Arduino
port = get_serial_port;

%set the data buffer size-- arduino returns packet w/ one command
pckLenSingleCmd = 5; % 5 bytes for packet with one command
cmd_len = 3;



fprintf('attempting to open port...\n');
try
    fopen(port);
    pause(2.5);
    fprintf('port successfully opened.\n');
    cleaner = onCleanup(@()fclose(port));
    pause;
    
    [packets, waits, lengths] = mexPacketBytesFromPlayfile();
    temp = size(packets);
    npackets = temp(2)
    ncmds = zeros(size(lengths));

    for i=1:npackets
        fprintf('packet %d (post-delay=%dms):\n', i,waits(1,i));
        ncmds(1,i) = (lengths(1,i)-3)/cmd_len;
        dec2bin(uint8(packets(1:lengths(1,i), i)), 8);  
        pcklen=lengths(1,i);
        for j=1:pcklen
            fprintf('%d ',packets(j,i));
        end
        fprintf('\n');
    end
    pause;
    
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
    
catch err
    fclose(port);
    error(getReport(err));
end

fclose(port);

