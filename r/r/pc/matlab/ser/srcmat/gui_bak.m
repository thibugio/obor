% ~matlab~ gui for the catheter controller...
function gui_bak
    % main window location and dimensions
    xpos_main = 300; % from left
    ypos_main = 500; % from bottom 
    width_main = 800;
    height_main = 600;
    
    % create the main window
    window_main = figure('Visible', 'off', 'Position', [xpos_main, ypos_main, width_main, height_main]);

    % three panels: File Management, Build Packets, Command Center
    filePanel = uipanel('Parent', window_main, 'Title', 'Manage Files', 'Units', 'normalized',...
        'Position', [0, .75, 1, .25]);
    packetPanel = uipanel('Parent', window_main, 'Title', 'Construct Packets', 'Units', 'normalized',...
        'Position', [0, .5, 1, .5]);
    packetPanelTG = uitabgroup('Parent', packetPanel, 'Title', 'Construct Packets', 'Units', 'normalized',...
        'Position', [0, .5, 1, .5]);
    cmdPanel = uipanel('Parent', window_main, 'Title', 'Send Commands', 'Units', 'normalized',...
        'Position', [0, 0, 1, .25]);
    
    % window divisions
    gridX=10; % number of width-wise window divisions
    gridY=10; % number of height-wise window divisions

    % push button dimensions
    bw = int(width/gridX)-2;
    bh = int(height/gridY)-2;
    
    % push button locations
    bX = zeros(gridX);
    bY = zeros(gridY);
    for i=1:gridX
        bX(i) = i*int(width/gridX);
    end
    for i=1:gridY
        bY(i) = i*int(height/gridY);
    end

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
%    quit = uicontrol('Style', 'pushbutton', 'String', 'Quit', 'Position', [bX(1), bY(GridY), bw, bh], 'Callback', {@onQuit});
%    runPlayfile = uicontrol('Style', 'pushbutton', 'String', 'Run Playfile', 'Position', [bX(2), bY(1), bw, bh], 'Callback', {@onRunPlayfile});

    % create toggle buttons for Commands

    % initialize the GUI
    window_main.Units = 'normalized';    % resize components automatically
    loadPlayfile.Units = 'normalized';
    runPlayfile.Units = 'normalized';
    addPacket.Units = 'normalized';
    sendPackets.Units = 'normalized';
    sendReset.Units = 'normalized';
    quit.Units = 'normalized';
    saveToPlayfile.Units = 'normalized';

    % set window title
    window_main.Name = 'Catheter Control GUI';

    % set window position
    movegui(window_main, 'center');

    % set window visibility
    set(window, 'Visible', 'on');

    
    % callback methods 

    function onLoadPlayfile(source, eventdata)
    end

    function onRunPlayfile(source, eventdata)
    end

    function onAddPacket(source, eventdata)
    end

    function onSendPackets(source, eventdata)
    end

    function onSendReset(source, eventdata)
    end

    function onQuit(source, eventdata)
    end

    function onStartLog(source, eventdata)
    end

    function onStopLog(source, eventdata)
    end
end


