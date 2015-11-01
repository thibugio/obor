% ~matlab~ gui for the catheter controller...
function catheter_guix2
    NCHANNELS=6;
    CMDLEN=3;
    PRELEN=1;
    POSTLEN=1;
    CHKLEN=1;
    maxPckLen = CMDLEN*NCHANNELS + PRELEN + POSTLEN + CHKLEN;
    
    maxPackets = 8;
    
    %'global' vars for callback methods
    filePaths = {'','',''};
    npackets = 0;
    packets = zeros(maxPckLen, maxPackets);
    delays = zeros(1,maxPackets);
    pckLens = zeros(1,maxPackets);    
    channelCurrents = zeros(1,maxPackets);
    port = 0; 
    
    logging = 0;
    playfileLoaded = 0;
    cmdWindowEmpty = 1;
    bufferLoaded = 0;
    
    sourcePlayfile = 0;
    sourceCmdWindow = 1;
    sourceNone = 2;
    
    % main window location and dimensions
    
    xpos_main = 0; % from left
    ypos_main = .25; % from bottom 
    width_main = .7;
    height_main = .7;
    
    p1h = 0.15;  % panel 1 relative height
    p2h = 0.7;
    p3h = 1-p1h-p2h;
    
    % create the main window
    window_main = figure('Units', 'normalized', 'Visible', 'off',...
        'Position', [xpos_main, ypos_main, width_main, height_main],...
        'Name', 'Catheter Control GUI', 'NumberTitle', 'off',...
        'WindowStyle', 'modal', 'CloseRequestFcn', @myCloseReqFcn);

    % three panels: File Management, Build Packets, Command Center
    filePanel = uipanel('Parent', window_main, 'Title', 'Manage Files', 'Units', 'normalized',...
        'Position', [0, 1-p1h, 1, p1h]);
    packetPanel = uipanel('Parent', window_main, 'Title', 'Construct Packets', 'Units', 'normalized',...
        'Position', [0, p3h, 1, p2h]);
    packetPanelTG = uitabgroup('Parent', packetPanel, 'Units', 'normalized',...
        'Position', [0, 1/(NCHANNELS+2), 1, 1-1/(NCHANNELS+2)]);
    cmdPanel = uipanel('Parent', window_main, 'Title', 'Send Commands', 'Units', 'normalized',...
        'Position', [0, 0, 1, p3h]);
    
    % button dimensions relative to parent panel
    bPadX = 0.01;
    bPadY = 0.01;
    bNormX = 0.2;
    bNormY = 0.4;

    % create push buttons and text fields
        % buttons in File-management panel
    loadPlayfilePath = uicontrol('Parent', filePanel, 'Style', 'edit', 'Min', 0, 'Max', 0, 'BackgroundColor', 'w',...
        'HorizontalAlignment', 'left', 'String', filePaths{1},...
        'Units', 'normalized', 'Position', [bPadX, 2*bPadY+bNormY, bNormX, bNormY], 'Callback', {@onLoadPlayfilePath},...
        'ButtonDownFcn', @onLoadPlayfilePathButtonDown);
    loadPlayfile = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Load Playfile', 'BackgroundColor', 'r',...
        'Units', 'normalized', 'Position', [bPadX, bPadY, bNormX/2, bNormY], 'Callback', {@onLoadPlayfile});
    unloadPlayfile = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Unload Playfile',...
        'Units', 'normalized', 'Position', [bPadX+bNormX/2, bPadY, bNormX/2, bNormY], 'Callback', {@onUnloadPlayfile});
    savePlayfilePath = uicontrol('Parent', filePanel, 'Style', 'edit', 'Min', 0, 'Max', 0, 'BackgroundColor', 'w',...
        'HorizontalAlignment', 'left', 'String', filePaths{2},...
        'Units', 'normalized', 'Position', [2*bPadX+bNormX, 2*bPadY+bNormY, bNormX, bNormY], 'Callback', {@onSavePlayfilePath});
    saveToPlayfile = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Save to Playfile',...
        'Units', 'normalized', 'Position', [2*bPadX + bNormX, bPadY, bNormX, bNormY],...
        'Callback', {@onSaveToPlayfile});
    logfilePath = uicontrol('Parent', filePanel, 'Style', 'edit', 'Min', 0, 'Max', 0, 'BackgroundColor', 'w',...
        'HorizontalAlignment', 'left', 'String', filePaths{3},...
        'Units', 'normalized', 'Position', [3*bPadX+2*bNormX, 2*bPadY+bNormY, bNormX, bNormY], 'Callback', {@onLogfilePath});
    startLog = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Start Status Log',...
        'Units', 'normalized', 'Position', [3*bPadX + 2*bNormX, bPadY, bNormX, bNormY],...
        'Callback', {@onStartLog});
    stopLog = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Stop Status Log',...
        'Units', 'normalized', 'Position', [4*bPadX + 3*bNormX, bPadY, bNormX, bNormY],...
        'Callback', {@onStopLog});
        % buttons in Packet-building panel
    addPacket = uicontrol('Parent', packetPanel, 'Style', 'pushbutton', 'String', 'Add Packet',...
        'Units', 'normalized', 'Position', [bPadX, 0, bNormX, 1/(NCHANNELS+1)/2], 'Callback', {@onAddPacket});
    previewPackets = uicontrol('Parent', packetPanel, 'Style', 'pushbutton', 'String', 'Preview Packets',...
        'Units', 'normalized', 'Position', [2*bPadX+bNormX, 0, bNormX, 1/(NCHANNELS+1)/2], 'Callback', {@onPreviewPackets});
    clearCmdWindow = uicontrol('Parent', packetPanel, 'Style', 'pushbutton', 'String', 'Clear Commands',...
        'Units', 'normalized', 'Position', [3*bPadX+2*bNormX, 0, bNormX, 1/(NCHANNELS+1)/2], 'Callback', {@onClearCmdWindow});
        % buttons in Command-center panel
    sendPackets = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Send Packets',...
        'Units', 'normalized', 'Position', [bPadX, 2*bPadY+bNormY, bNormX, bNormY], 'Callback', {@onSendPackets});
    sendReset = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Send Global Reset',...
        'Units', 'normalized', 'Position', [2*bPadX+bNormX, 2*bPadY+bNormY, bNormX, bNormY], 'Callback', {@onSendReset});
%     runPlayfile = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Run Playfile',...
%         'Units', 'normalized', 'Position', [3*bPadX+2*bNormX, 2*bPadY+bNormY, bNormX, bNormY],...
%         'Callback', {@onRunPlayfile});
    packetStatusLights = zeros(maxPackets);
    pslw = (1-(5*bPadX + 3*bNormX))/maxPackets;
    for i=1:maxPackets
        packetStatusLights(i) = uicontrol('Parent', cmdPanel, 'Style', 'text', 'String', num2str(i),...
            'Units', 'normalized', 'Position', [4*bPadX+3*bNormX+i*pslw, 2*bPadY+bNormY, bNormX, bNormY],...
            'Visible', 'off');
    end
    connect = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Connect',...
        'Units', 'normalized', 'Position', [bPadX, bPadY, bNormX, bNormY],...
        'BackgroundColor', 'r', 'Callback', {@onConnect});
    exit = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Exit',...
        'Units', 'normalized', 'Position', [2*bPadX+bNormX, bPadY, bNormX, bNormY],...
        'Callback', {@onExit});

    % add a tab for the first packet
    packet1 = uitab('Parent', packetPanelTG, 'Title', 'Packet 1');   
    globalChannel = uicontrol('Parent', packet1, 'Style', 'checkbox', 'String', 'Global',...
        'Units', 'normalized', 'Position', [0,1-(1/(NCHANNELS+1)),0.1,1/(NCHANNELS+1)],...
        'Callback', @onGlobalChannel);
    
    % construct command panels
    channelCmdPanels = zeros(NCHANNELS);
    cmdButtonLabels = cellstr(['Include Channel';...
                               'Pol.           ';...
                               'Ena.           ';...
                               'Upd.           ';...
                               'Dir.           ']);
    channelButtons = zeros(NCHANNELS, size(cmdButtonLabels, 1));
    channelCurrentTFs = zeros(NCHANNELS);
    labelw = 1/(1+size(cmdButtonLabels,1));
    
    for i=1:NCHANNELS
        channelCmdPanels(i) = uipanel('Parent', packet1, 'Title', strcat('Channel ', num2str(i)), 'Units', 'normalized',...
            'Position', [0, 1-(i+1)/(NCHANNELS+1), 1, 1/(NCHANNELS+0)]);
        for j=1:size(cmdButtonLabels,1)+1
            switch j
                case 1
                    channelButtons(i,j) = uicontrol('Parent', channelCmdPanels(i), 'Style', 'checkbox', 'String', cmdButtonLabels{j},...
                        'Units', 'normalized', 'Position', [(j-1)*labelw,bPadY,labelw,1-bPadY],...
                        'Callback', {@onChannelButtons, i}, 'Min', 0, 'Max', 1);
                case 2
                    uicontrol('Parent', channelCmdPanels(i), 'Style', 'text', 'String', 'Current (res)',...
                        'Units', 'normalized', 'Position', [(j-1)*labelw,bPadY,0.25*labelw,1-bPadY]);
                    channelCurrentTFs(i) = uicontrol('Parent', channelCmdPanels(i), 'Style', 'edit','String','0',...% 'String', channelCurrents{i},...
                        'Min', 0, 'Max', 0, 'BackgroundColor', 'w',...
                        'Units', 'normalized', 'Position', [(j-1)*labelw+(0.25*labelw),bPadY,0.75*labelw,1-bPadY],...
                        'Callback', {@onChannelCurrent, i});
                otherwise
                    channelButtons(i,j-1) = uicontrol('Parent', channelCmdPanels(i), 'Style', 'checkbox', 'String', cmdButtonLabels{j-1},...
                        'Units', 'normalized', 'Position', [0.5+(j-3)/8,bPadY,labelw,1-bPadY],...
                        'Callback', {@onChannelButtons, i}, 'Min', 0, 'Max', 1);
            end
        end
    end

    % set window position
    movegui(window_main, 'center');

    % set window visibility
    set(window_main, 'Visible', 'on');
    
    identifyPacketSource(sourceNone);

    
    % callback methods 
    
    function onLoadPlayfilePath(source, eventdata) 
        filePaths{1} = get(source, 'String');
        set(source, 'String', filePaths{1});
    end        

    function onLoadPlayfilePathButtonDown(source, eventdata)
        [fname, pathname, fi] = uigetfile('*.*');
        if fname > 0
            filePaths{1} = strcat(pathname,fname);
            set(loadPlayfilePath, 'String', filePaths{1});
        end
        set(source, 'String', filePaths{1});
    end

    function onSavePlayfilePath(source, eventdata)
        [fname, pathname, fi] = uigetfile('*.*');
        if fname > 0
            filePaths{2} = strcat(pathname,fname);
            set(loadPlayfilePath, 'String', filePaths{2});
        end
        set(source, 'String', filePaths{2});
    end

    function onLogfilePath(source, eventdata)
        [fname, pathname, fi] = uigetfile('*.*');
        if fname > 0
            filePaths{3} = strcat(pathname,fname);
            set(loadPlayfilePath, 'String', filePaths{3});
        end
        set(source, 'String', filePaths{3});
    end

    function onLoadPlayfile(source, eventdata)
        if length(filePaths{1}) == 0
            msgbox('enter load-playfile path!');
        else
            [npackets, packets, delays, pckLens, channelCurrents] = m_load_playfile(filePaths{1}, maxPackets);
            playfileLoaded = 1;
            set(source, 'BackgroundColor', 'g');
        end
    end

    function onUnloadPlayfile(source, eventdata)
        reset_buffers();
    end

    function onSaveToPlayfile(source, eventdata)
        if length(filePaths{2}) == 0
            msgbox('enter save-playfile path!');
        else
            save_command_window_as_playfile(filePaths{2});
        end
    end

%     function onRunPlayfile(source, eventdata)
%         fprintf('running playfile...\n');
%         if playfileLoaded == 0
%             msgbox('load a playfile first!');
%         else
%             send_packets();
%             playfileLoaded = 0;
%         end
%     end

    function onAddPacket(source, eventdata)
        msgbox('not yet implemented!\n');
    end

    function onPreviewPackets(source, eventdata)
        if playfileLoaded == 1
            [npackets, packets, delays, pckLens, channelCurrents] = m_load_playfile(filePaths{1}, maxPackets);
            identifyPacketSource(sourcePlayfile);
        else
            if cmdWindowEmpty == 0
                tempf = strcat(pwd(),'\spqrdvtz.play');
                save_command_window_as_playfile(tempf);
                [npackets, packets, delays, pckLens, channelCurrents] = m_load_playfile(tempf, maxPackets);
                identifyPacketSource(sourceCmdWindow);
            else
                msgbox('no commands loaded!');
            end
        end
        bufferLoaded = 1;
    end

    function onSendPackets(source, eventdata)
        if bufferLoaded == 1
            send_packets();
        else
            msgbox('no commands entered!');
        end
    end

    function onSendReset(source, eventdata)
        [npackets, packets, delays, pckLens, channelCurrents] = m_get_reset_packet(maxPackets);
        send_packets();
    end

    function onStartLog(source, eventdata)
        if logging == 1
            msgbox('already logging!');
        else
            if length(filePaths{3}) == 0
                msgbox('enter status-log path!');
                [fname, pathname, fi] = uigetfile('*.*');
                if fname > 0
                    filePaths{3} = strcat(pathname,fname);
                    set(logfilePath, 'String', filePaths{3});
                end
            else
                msgbox('starting status logging...');
            end
        end
    end

    function onStopLog(source, eventdata)
        if logging == 1
            msgbox('stopping status logging...');
        else
            msgbox('status logging was not started!');
        end
    end

    function onConnect(source, eventdata)
        try
            port = get_serial_port();
            fopen(port);
            pause(2.5);
            msgbox('port successfully opened.\n');
            set(source, 'BackgroundColor', 'g');
            set(source, 'String', 'Connected');
        catch err
            msgbox(getReport(err));
        end
    end

    function onExit(source, eventdata)
        myCloseReqFcn;
    end

    function myCloseReqFcn(source, eventdata)
        try
            fclose(port);
        catch err
            msgbox(getReport(err));
        end
        closereq;
    end

    function onChannelCurrent(source, eventdata, chan)
        val = str2num(get(source, 'String'));
        if val < 0
            set(source, 'String', '0');
        else 
            if val > 4095
                set(source, 'String', '4095');
            end
        end
        channelCurrents(1,chan) = str2num(get(source, 'String'));
    end

    function onChannelButtons(source, eventdata, chan)
        includeChannel = 0;
        for k=1:NCHANNELS
            if source == channelButtons(k,1)
                includeChannel = 1;
            end
        end
        if includeChannel == 1
            if get(source, 'Value') == 1
                cmdWindowEmpty = 0;
                identifyPacketSource(sourceCmdWindow);
            else
                for k=1:NCHANNELS
                    cmdWindowEmpty = 1;
                    if get(channelButtons(k,1), 'Value') == 1
                        cmdWindowEmpty = 0;
                        break;
                    end
                end
                if cmdWindowEmpty == 0
                    identifyPacketSource(sourceCmdWindow);
                else
                    set(globalChannel, 'Value', 0);
                    if (playfileLoaded)
                        identifyPacketSource(sourcePlayfile);
                    else
                        identifyPacketSource(sourceNone);
                    end
                end
            end                    
        end %endif includeChannel == 1            
    end

    function onGlobalChannel(source, eventdata)
        if get(source, 'Value') == 1            
            for k=1:NCHANNELS
                set(channelButtons(k,1), 'Value', 1);
            end
            cmdWindowEmpty = 0;
            identifyPacketSource(sourceCmdWindow);
        else
           for k=1:NCHANNELS
                set(channelButtons(k,1), 'Value', 0);
            end
            cmdWindowEmpty = 1;
            if (playfileLoaded)
                identifyPacketSource(sourcePlayfile);
            else
                identifyPacketSource(sourceNone);
            end 
        end
    end

    function onClearCmdWindow(source, eventdata)
        cmdWindowEmpty = 0;
        if (playfileLoaded)
            identifyPacketSource(sourcePlayfile)
        else
            identifyPacketSource(sourceNone)
        end
        set(globalChannel, 'Value', 0);
        for k=1:NCHANNELS
            for n=1:size(cmdButtonLabels,1)
                set(channelButtons(k,n), 'Value', 0);
            end
        end
    end

    function reset_buffers()
        npackets = 0;
        packets = zeros(maxPckLen, maxPackets);
        delays = zeros(1,maxPackets);
        pckLens = zeros(1,maxPackets);    
        channelCurrents = zeros(1,maxPackets);
        
        playfileLoaded = 0;
        bufferLoaded = 0;
        cmdWindowEmpty = cmdWindowIsEmpty();
        
        identifyPacketSource(sourceNone);
    end

    function isEmpty = cmdWindowIsEmpty()
        isEmpty = 1;
        for k=1:NCHANNELS
            if get(channelButtons(k, 1),'Value') == 1
                isEmpty = 0;
                break;
            end
        end
    end

    function send_packets()
        try
            %m_send_packets(npackets, packets, delays, pckLens, port);
            ncmds = zeros(1,npackets);
            for k=1:npackets
                ncmds(1,k) = (lengths(1,k)-3)/CMDLEN;
                set(packetStatusLights(k), 'BackgroundColor', 'r');
                set(packetStatusLights(k), 'Visible', 'on');
            end

            pause_ms = 100; 

            for k=1:npackets
                fwrite(port, packets(1:lengths(1,k), k), 'uint8');
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
                for n=1:bytes
                    fprintf('%d ',ret(n));
                end
                fprintf('\n');
                switch bytes
                    case 1
                        fprintf('error in transmission: ');
                        fprintf('error code = %d\n', ret-bitshift(pckIndex,4));
                    case 2
                        fprintf('error in transmission: ');
                        fprintf('error code = %d\n', ret(2)-bitshift(pckIndex,4));
                    case 2*ncmds(1,k) + 2
                        fprintf('transmission successful\n');
                        set(packetStatusLights(k), 'BackgroundColor', 'g');
                    otherwise
                        fprintf('not all commands executed\n');
                end
                new_wait_ms = waits(1,k) - pause_cnt_ms;
                if (new_wait_ms < 0)
                  new_wait_ms = 0;
                end
                pause(new_wait_ms/1000);
            end
            for k=1:npackets
                set(packetStatusLights(k), 'Visible', 'off');
            end
            reset_buffers();
        catch err
            msgbox(getReport(err));
        end
    end

    function [ncmds] = save_command_window_as_playfile(path)
        fd = fopen(path, 'w');
        ncmds = 0;
        for k=1:NCHANNELS
            if get(channelButtons(k, 1),'Value') == 1
                poll = get(channelButtons(k,2), 'Value');
                ena = get(channelButtons(k,3), 'Value');
                upd = get(channelButtons(k,4), 'Value');
                dir = get(channelButtons(k,5), 'Value');
                fprintf(fd, '%d, %d, %d, %d\n', k, channelCurrents(1,k), dir, 0);
                ncmds = ncmds + 1;
            end
        end
        fclose(fd);
    end

    function identifyPacketSource(source)
        if (source == sourcePlayfile)
            set(loadPlayfile, 'BackgroundColor', 'g');
            set(packet1, 'BackgroundColor', 'r');
        else
            if (source == sourceCmdWindow)
                set(loadPlayfile, 'BackgroundColor', 'r');
                set(packet1, 'BackgroundColor', 'g');
            else
                if (source == sourceNone)
                    set(loadPlayfile, 'BackgroundColor', 'r');
                    set(packet1, 'BackgroundColor', 'r');
                end
            end
        end                
    end

end


