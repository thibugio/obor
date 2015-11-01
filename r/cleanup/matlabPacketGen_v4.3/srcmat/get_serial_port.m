function [ port ] = get_serial_port()
    %Create serial object for Arduino from available serial ports
    hwinfo = instrhwinfo('serial');
    temp = size(hwinfo.AvailableSerialPorts);
    num_available_ports = temp(2);
    if (num_available_ports == 0)
        error('no available serial ports');
    else
        port_name = char(hwinfo.AvailableSerialPorts(num_available_ports));
        port = serial(port_name); 
        port.Terminator='CR';
        port.BytesAvailableFcnCount=1;
    end   
end

