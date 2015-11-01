import java.net.*;
import java.io.*;
import java.util.*;

class socketSender {
    private int dest_port;
    private int local_port;
    private Inet4Address dest_ip;
    private Inet4Address local_ip;
    private DatagramSocket sock;

    private void setAddressByName(String remoteAddress, String name) {
        try {
            Inet4Address temp = (Inet4Address)InetAddress.getByName(remoteAddress);
            if (name.equals("local"))
                local_ip = temp;
            else if(name.equals("remote"))
                dest_ip = temp;
        } catch (UnknownHostException e) {
                utils.exceptMsg("WARNING: failed to resolve address: ", e);
        }
    } 

    private void setLocalAddress () {
        try {
            local_ip = (Inet4Address)InetAddress.getLocalHost();        
        } catch (UnknownHostException e) {
            utils.exceptMsg("WARNING: failed to resolve address: ",e);
        }
    } 

    int getLocalPort() { return this.local_port; }
    Inet4Address getLocalAddress() { return this.local_ip; }
    String getLocalAddressString() { 
        return this.local_ip.getHostAddress(); 
    }
    
    int getRemotePort() { return this.dest_port; }
    void setRemotePort(int port) {this.dest_port = port;};

    Inet4Address getRemoteAddress() { return this.dest_ip; }
    String getRemoteAddressString() { 
        return this.dest_ip.getHostAddress(); 
    }
    void setRemoteAddress(String address) {
        setAddressByName(address, "remote");
    }

    boolean bind () {
        utils.assertErr (local_ip != null, "WARNING: local address unresolved.\n");
        try {
            sock.bind (new InetSocketAddress(local_ip, local_port));
        } catch (IOException e) {
            utils.exceptMsg("WARNING: ",e);
        }
        return sock.isBound();
    }

    boolean bindLocal () {
        if (local_ip == null) {
            setLocalAddress(); 
        }
        return bind();
    }

    boolean connect () {
        utils.assertErr (dest_ip != null, "WARNING: remote address unresolved.\n");
        try {
            sock.connect (new InetSocketAddress(dest_ip, dest_port)); 
            return sock.isConnected();
        } catch (IOException e) {
            utils.exceptMsg("WARNING: ",e);
            return false;
        }
    }

    boolean connectToAddress (String addr) {
        if (dest_ip == null) {
            setAddressByName(addr, "remote"); 
        }
        return connect();
    }

    void setReuseAddress (boolean on) {
        try {
            sock.setReuseAddress(on);
        } catch (SocketException e) {
            utils.exceptMsg("WARNING: ",e);
        }
    }

    boolean close () {
        sock.close();
        return sock.isClosed();
    }

    

    boolean isPosAck(DatagramPacket p) {
        byte[] pdata = p.getData();
        return (pdata[pdata.length-1] > 0);
    }

    void sendPacket1 (String field, byte[] databytes) {
        byte fieldbytes[] = field.getBytes();
        byte pbytes[] = new byte[fieldbytes.length + databytes.length + 1];
        System.arraycopy(fieldbytes, 0, pbytes, 0, fieldbytes.length);
        pbytes[fieldbytes.length] = ' ';
        System.arraycopy(databytes, 0, pbytes, fieldbytes.length+1, databytes.length);
        DatagramPacket p = new DatagramPacket(pbytes, pbytes.length, dest_ip, dest_port);
        try{       
            sock.send(p);
            byte ackbytes[] = new byte[1];
            DatagramPacket pack = new DatagramPacket(ackbytes, ackbytes.length);
            sock.receive(pack);
            while (!isPosAck(pack)) {
                if (utils.DEBUG)
                    System.out.println("received negative ACK.\n");
                sock.send(p);
                sock.receive(pack);
            }
            if (utils.DEBUG) { 
                System.out.print("received "+pack.getLength()+" bytes from remote server: ");
                for (byte b : pack.getData()) System.out.print("\t"+b);
                System.out.println();
            }
        } catch (IOException e) {
            utils.exceptMsg("WARNING: ", e);
        }
    }
    void sendPacket1 (String field, int data) {
        sendPacket1(field, utils.intToByteArray_BE(new Integer(data)));
    }
    void sendPacket1 (String field, boolean data) {
        if (data)
            sendPacket1 (field, 1);
        else 
            sendPacket1 (field, 0);
    }
    void sendPacket1 (String field, String data) {
        sendPacket1 (field, data.getBytes());
    }
    
    void sendPacket2 (String field, int data) {
        byte fieldbytes[] = field.getBytes();
        byte databytes[] = utils.intToByteArray_BE(new Integer(data));
        DatagramPacket pfield = new DatagramPacket(fieldbytes, fieldbytes.length, dest_ip, dest_port);
        DatagramPacket pdata = new DatagramPacket(databytes, databytes.length, dest_ip, dest_port);
        try{       
            sock.send(pfield);
            byte ack1bytes[] = new byte[1];
            DatagramPacket pack1 = new DatagramPacket(ack1bytes, ack1bytes.length);
            sock.receive(pack1);
            if (utils.DEBUG)
                System.out.println("received "+pack1.getLength()+" bytes from remote server.\n");

            sock.send(pdata);
            byte ack2bytes[] = new byte[1];
            DatagramPacket pack2 = new DatagramPacket(ack2bytes, ack2bytes.length);
            sock.receive(pack2);
            if (utils.DEBUG)
                System.out.println("received "+pack2.getLength()+" bytes from remote server.\n");
        } catch (IOException e) {
            utils.exceptMsg("WARNING: ", e);
        }
    }

    socketSender(int localPort, int remotePort) {
        this.dest_port = remotePort;
        this.local_port = localPort;
        try {
            this.sock = new DatagramSocket();
        } catch (SocketException e) {
            utils.exceptMsg("WARNING: ", e);
        }
    }

    socketSender(int localPort, int remotePort, String localAddress, String remoteAddress) {
        this(localPort, remotePort);
        setAddressByName(localAddress, "local");
        setAddressByName(remoteAddress, "remote");
    }

    socketSender(int localPort, int remotePort, String remoteAddress) {
        this(localPort, remotePort);
        setAddressByName(remoteAddress, "remote");
        setLocalAddress();
    }

    socketSender(int remotePort, String remoteAddress) {
        this(-1, remotePort, remoteAddress);
    }

    /*
    public static void main(String args[]) {
        Scanner scan = new Scanner(System.in);
        System.out.print("enter remote host address >> ");
        //String remoteAddress = scan.nextLine();
        String remoteAddress = "192.168.0.107";        
        System.out.print("enter remote host port >> ");
        //int remotePort = scan.nextInt();
        int remotePort = 12345;
        
        try {
            SecurityManager sm = System.getSecurityManager();
            if (sm != null) sm.checkAccept(remoteAddress, remotePort);
        } catch (SecurityException e) {
            if (utils.DEBUG)
                System.out.println("security manager: cannot accept connection from the requested host.\n");
        }
        socketSender ss = new socketSender (remotePort, remoteAddress);
        //utils.assertFail(ss.bindLocal(), "ERROR: socket failed to bind to local endpoint.\n");
        utils.assertFail(ss.connect(), "ERROR: socket failed to connect to remote endpoint.\n");
        ss.setReuseAddress(true);
        
        String field = "field";
        int data = 0;
        while (data < 1000) {
            System.out.print("sending next packet...");
            ss.sendPacket1(field, data);
            if (utils.DEBUG)
                System.out.println("packet sent.\n");
            data+=100;
            try {
                Thread.currentThread().sleep(1000); //ms
            } catch (InterruptedException e) {
                utils.exceptMsg("WARNING: ",e);
            }
        }
        utils.assertFail(ss.close(), "ERROR: socket failed to close.\n");
    }*/
}
