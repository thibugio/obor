% ~matlab~ gui for the catheter controller...
function catheter_guix
    % main window location and dimensions
    xpos_main = 300; % from left
    ypos_main = 500; % from bottom 
    width_main = 800;
    height_main = 600;
    
    % create the main window
    window_main = figure('Units', 'normalized', 'Visible', 'off',...
        'Position', [xpos_main, ypos_main, width_main, height_main]);

    % three panels: File Management, Build Packets, Command Center
    filePanel = uipanel('Parent', window_main, 'Title', 'Manage Files', 'Units', 'normalized',...
        'Position', [0, .75, 1, .25]);
    packetPanel = uipanel('Parent', window_main, 'Title', 'Construct Packets', 'Units', 'normalized',...
        'Position', [0, .5, 1, .5]);
    packetPanelTG = uitabgroup('Parent', packetPanel, 'Units', 'normalized',...
        'Position', [0, 0, 1, .85]);
    cmdPanel = uipanel('Parent', window_main, 'Title', 'Send Commands', 'Units', 'normalized',...
        'Position', [0, 0, 1, .25]);
    
    % button dimensions relative to parent panel
    bPadX = 0.05;
    bPadY = 0.05;
    bNormX = 0.2;
    bNormY = 0.3;

    % create push buttons
        % buttons in File-management panel
    loadPlayfile = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Load Playfile',...
        'Units', 'normalized', 'Position', [bPadX, bPadY, bNormX, bNormY], 'Callback', {@onLoadPlayfile});
    saveToPlayfile = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Save to Playfile',...
        'Units', 'normalized', 'Position', [2*bPadX + bNormX, bPadY, bNormX, bNormY],...
        'Callback', {@onSaveToPlayfile});
    startLog = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Start Status Log',...
        'Units', 'normalized', 'Position', [3*bPadX + 2*bNormX, bPadY, bNormX, bNormY],...
        'Callback', {@onStartLog});
    stopLog = uicontrol('Parent', filePanel, 'Style', 'pushbutton', 'String', 'Stop Status Log',...
        'Units', 'normalized', 'Position', [4*bPadX + 3*bNormX, bPadY, bNormX, bNormY],...
        'Callback', {@onStopLog});
        % buttons in Packet-building panel
    addPacket = uicontrol('Parent', packetPanel, 'Style', 'pushbutton', 'String', 'Add Packet',...
        'Units', 'normalized', 'Position', [bPadX/2, bPadY/2, bNormX/2, bNormY/2], 'Callback', {@onAddPacket});
        % buttons in Command-center panel
    sendPackets = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Send Packets',...
        'Units', 'normalized', 'Position', [bPadX, bPadY, bNormX, bNormY], 'Callback', {@onSendPackets});
    sendReset = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Send Reset',...
        'Units', 'normalized', 'Position', [2*bPadX+bNormX, bPadY, bNormX, bNormY], 'Callback', {@onSendReset});
    runPlayfile = uicontrol('Parent', cmdPanel, 'Style', 'pushbutton', 'String', 'Send Packets',...
        'Units', 'normalized', 'Position', [3*bPadX+2*bNormX, bPadY, bNormX, bNormY],...
        'Callback', {@onSendPackets});

    % add a tab for the first packet
    packet1 = uitab('Parent', packetPanelTG, 'Title', 'Packet 1');

    % set window title
    window_main.Name = 'Catheter Control GUI';

    % set window position
    movegui(window_main, 'center');

    % set window visibility
    set(window_main, 'Visible', 'on');

    
    % callback methods 

    function onLoadPlayfile(source, eventdata)
        fprintf('loading playfile...\n');
    end

    function onRunPlayfile(source, eventdata)
        fprintf('running playfile...\n');
    end

    function onAddPacket(source, eventdata)
        fprintf('adding packet...\n');
    end

    function onSendPackets(source, eventdata)
        fprintf('sending packets...\n');
    end

    function onSendReset(source, eventdata)
        fprintf('sending reset command...\n');
    end

    function onStartLog(source, eventdata)
        fprintf('starting status logging...\n');
    end

    function onStopLog(source, eventdata)
        fprintf('stopping status logging...\n');
    end
end


