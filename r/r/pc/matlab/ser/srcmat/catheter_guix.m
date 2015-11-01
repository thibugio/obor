% ~matlab~ gui for the catheter controller...
function catheter_guix
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
        'Units', 'normalized', 'Position', [bPadX, 2*bPadY+bNormY, bNormX, bNormY], 'Callback', {@onLoadPlayfilePath});
    loadPlayfile = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Load Playfile',...
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
        % buttons in Command-center panel
    sendPackets = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Send Packets',...
        'Units', 'normalized', 'Position', [bPadX, 2*bPadY+bNormY, bNormX, bNormY], 'Callback', {@onSendPackets});
    sendReset = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Send Reset',...
        'Units', 'normalized', 'Position', [2*bPadX+bNormX, 2*bPadY+bNormY, bNormX, bNormY], 'Callback', {@onSendReset});
    runPlayfile = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Run Playfile',...
        'Units', 'normalized', 'Position', [3*bPadX+2*bNormX, 2*bPadY+bNormY, bNormX, bNormY],...
        'Callback', {@onRunPlayfile});
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
    cmdPanels = zeros(NCHANNELS);
    cmdButtonLabels = cellstr(['Include Channel';...
                               'Pol.           ';...
                               'Ena.           ';...
                               'Upd.           ';...
                               'Dir.           ']);
    channelButtons = zeros(NCHANNELS, size(cmdButtonLabels, 1));
    channelCurrentTFs = zeros(NCHANNELS);
    labelw = 1/(1+size(cmdButtonLabels,1));
    
    for i=1:NCHANNELS
        cmdPanels(i) = uipanel('Parent', packet1, 'Title', strcat('Channel ', num2str(i)), 'Units', 'normalized',...
            'Position', [0, 1-(i+1)/(NCHANNELS+1), 1, 1/(NCHANNELS+0)]);
        for j=1:size(cmdButtonLabels,1)+1
            switch j
                case 1
                    channelButtons(i,j) = uicontrol('Parent', cmdPanels(i), 'Style', 'checkbox', 'String', cmdButtonLabels{j},...
                        'Units', 'normalized', 'Position', [(j-1)*labelw,bPadY,labelw,1-bPadY],...
                        'Callback', {@onChannelButtons, i});
                case 2
                    uicontrol('Parent', cmdPanels(i), 'Style', 'text', 'String', 'Current (res)',...
                        'Units', 'normalized', 'Position', [(j-1)*labelw,bPadY,0.25*labelw,1-bPadY]);
                    channelCurrentTFs(i) = uicontrol('Parent', cmdPanels(i), 'Style', 'edit','String','0',...% 'String', channelCurrents{i},...
                        'Min', 0, 'Max', 0, 'BackgroundColor', 'w',...
                        'Units', 'normalized', 'Position', [(j-1)*labelw+(0.25*labelw),bPadY,0.75*labelw,1-bPadY],...
                        'Callback', {@onChannelCurrent, i});
                otherwise
                    channelButtons(i,j-1) = uicontrol('Parent', cmdPanels(i), 'Style', 'checkbox', 'String', cmdButtonLabels{j-1},...
                        'Units', 'normalized', 'Position', [0.5+(j-3)/8,bPadY,labelw,1-bPadY],...
                        'Callback', {@onChannelButtons, i});
            end
        end
    end

    % set window position
    movegui(window_main, 'center');

    % set window visibility
    set(window_main, 'Visible', 'on');

    
    % callback methods 
    
    function onLoadPlayfilePath(source, eventdata)        
        filePaths{1} = get(source, 'String');
        fprintf('setting load playfile path: %s\n',filePaths{1});
        set(source, 'String', filePaths{1});
    end

    function onSavePlayfilePath(source, eventdata)
        filePaths{2} = get(source, 'String');
        fprintf('setting save playfile path: %s\n',filePaths{2});
        set(source, 'String', filePaths{2});
    end

    function onLogfilePath(source, eventdata)
        filePaths{3} = get(source, 'String');
        fprintf('setting logfile path: %s\n',filePaths{3});
        set(source, 'String', filePaths{3});
    end

    function onLoadPlayfile(source, eventdata)
        if length(filePaths{1}) == 0
            msgbox('enter load-playfile path!');
            [fname, pathname, fi] = uigetfile('*.*');
            if fname > 0
                filePaths{1} = strcat(pathname,fname);
                set(loadPlayfilePath, 'String', filePaths{1});
            end
        else
            fprintf('loading playfile: %s\n', filePaths{1});
            [npackets, packets, delays, pckLens, channelCurrents] = m_load_playfile(filePaths{1}, maxPackets);
            playfileLoaded = 1;
        end
    end

    function onUnloadPlayfile(source, eventdata)
        printf('unloading playfile...\n');
        reset_buffers();
        playfileLoaded = 0;
    end

    function onSaveToPlayfile(source, eventdata)
        if length(filePaths{2}) == 0
            msgbox('enter save-playfile path!');
            [fname, pathname, filterindex] = uigetfile('*.*');
            if fname > 0
                filePaths{2} = strcat(pathname,fname);
                set(savePlayfilePath, 'String', filePaths{2});
            end
        else
            fprintf('saving command window to playfile: %s\n', filePaths{2});
            save_command_window_as_playfile(filePaths{2});
        end
    end

    function onRunPlayfile(source, eventdata)
        fprintf('running playfile...\n');
        if playfileLoaded == 0
            msgbox('load a playfile first!');
        else
            send_packets();
            playfileLoaded = 0;
        end
    end

    function onAddPacket(source, eventdata)
        fprintf('adding packet...\n');
        %npackets = npackets + 1;
    end

    function onPreviewPackets(source, eventdata)
        if playfileLoaded == 1
            msgbox('reading from loaded playfile.');
            [temp_npackets, temp_packets, temp_delays, temp_pckLens, temp_channelCurrents] = m_load_playfile(filePaths{1}, maxPackets);
            m_preview_packets(temp_npackets, temp_packets, temp_delays, temp_pckLens);
        else
            msgbox('reading from command window.');
            tempf = strcat(pwd(),'\spqrdvtz.play');
            ncmds = save_command_window_as_playfile(tempf);
            if ncmds > 0
                fprintf('previewing packet bytes...\n');
                [temp_npackets, temp_packets, temp_delays, temp_pckLens, temp_channelCurrents] = m_load_playfile(tempf, maxPackets);
                m_preview_packets(temp_npackets, temp_packets, temp_delays, temp_pckLens);
            else
                msgbox('no commands loaded!');
            end
        end
    end

    function onSendPackets(source, eventdata)
        tempf = strcat(pwd(),'\spqrdvtz.play');
        ncmds = save_command_window_as_playfile(tempf);
        if ncmds > 0
            [npackets, packets, delays, pckLens, channelCurrents] = m_load_playfile(tempf, maxPackets);
            send_packets();
        else
            msgbox('no commands entered!');
        end
    end

    function onSendReset(source, eventdata)
        [npackets, packets, delays, pckLens, channelCurrents] = m_get_reset_packet(maxPackets);
        fprintf('sending reset command...\n');
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
                fprintf('starting status logging...\n');
            end
        end
    end

    function onStopLog(source, eventdata)
        if logging == 1
            fprintf('stopping status logging...\n');
        else
            msgbox('status logging was not started!');
        end
    end

    function onConnect(source, eventdata)
        try
        fprintf('connecting to arduino...\n');
        port = get_serial_port();
        fopen(port);
        pause(2.5);
        fprintf('port successfully opened.\n');
        set(source, 'BackgroundColor', 'g');
        catch err
            error(getReport(err));
            msgbox(getReport(err));
        end
    end

    function onExit(source, eventdata)
        fprintf('exiting application...\n');
        myCloseReqFcn;
    end

    function myCloseReqFcn(source, eventdata)
        fprintf('closing application...\n');
        try
            fclose(port);
        end
        closereq;
    end

    function onChannelCurrent(source, eventdata, chan)
        fprintf('editing channel current...\n');
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
        fprintf('changed a channel setting...\n');
    end

    function onGlobalChannel(source, eventdata)
        for k=1:NCHANNELS
            set(channelButtons(k,1), 'Value', 1);
        end
    end

    function reset_buffers()
        npackets = 0;
        packets = zeros(maxPckLen, maxPackets);
        delays = zeros(1,maxPackets);
        pckLens = zeros(1,maxPackets);    
        channelCurrents = zeros(1,maxPackets);
    end

    function send_packets()
        try
            fprintf('sending packets...\n');
            m_send_packets(npackets, packets, delays, pckLens, port);
            reset_buffers();
        catch err
            msgbox('no serial port connection!');
            error(getReport(err));
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

end


