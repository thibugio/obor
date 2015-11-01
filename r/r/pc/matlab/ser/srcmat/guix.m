function guix
    % main window location and dimensions
    xpos_main = 300; % from left
    ypos_main = 500; % from bottom 
    width_main = 800;
    height_main = 600;
    
    % create the main window
    window_main = figure('Units', 'normalized', 'Visible', 'off',...
        'Position', [xpos_main, ypos_main, width_main, height_main],...
        'Name', 'Catheter Control GUI');
    

    % set window position
    movegui(window_main, 'center');

    % set window visibility
    set(window_main, 'Visible', 'on');
end