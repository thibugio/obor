import java.net.*;
import java.io.*;
import java.util.*;

class sendThread extends Thread {
    String field;
    int data;
    OutputStream stream;

    sendThread(String field, int data, OutputStream stream) {
        this.field = field;
        this.data = data;
        this.stream = stream;
    }

    public void run() {
        try {
            BufferedWriter br = new BufferedWriter(new OutputStreamWriter(stream));
            br.write(field);
            br.write(data);
            br.flush();
            br.close();
        } catch (IOException e) {
            System.out.println("WARNING: "+e.getMessage());
        }
    }
}


class socketSender {
    private int dest_port;
    private int local_port;
    private Inet4Address dest_ip;
    private Inet4Address local_ip;
    private Socket sock;

    private void setAddressByName(String remoteAddress, String name) {
        try {
            Inet4Address temp = (Inet4Address)InetAddress.getByName(remoteAddress);
            if (name.equals("local"))
                local_ip = temp;
            else if(name.equals("remote"))
                dest_ip = temp;
        } catch (UnknownHostException e) {
            System.out.println("WARNING: failed to resolve address:\n");
            System.out.println(e.getMessage());
        }
    } 

    private void setLocalAddress () {
        try {
            local_ip = (Inet4Address)InetAddress.getLocalHost();        
        } catch (UnknownHostException e) {
            System.out.println("WARNING: failed to resolve address:\n");
            System.out.println(e.getMessage());
        }
    } 

    int getLocalPort() { return this.local_port; }

    int getRemotePort() { return this.dest_port; }

    Inet4Address getLocalAddress() { return this.local_ip; }

    Inet4Address getRemoteAddress() { return this.dest_ip; }

    boolean bind () {
        assertErr (local_ip != null, "WARNING: local address unresolved.\n");
        try {
            sock.bind (new InetSocketAddress(local_ip, local_port));
        } catch (IOException e) {
            System.out.println("WARNING: "+e.getMessage());
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
        assertErr (dest_ip != null, "WARNING: remote address unresolved.\n");
        try {
            sock.connect (new InetSocketAddress(dest_ip, dest_port)); 
        } catch (IOException e) {
            System.out.println("WARNING: "+e.getMessage());
        }
        return sock.isConnected();
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
            System.out.println("WARNING: "+e.getMessage());
        }
    }

    void setKeepAlive (boolean on) {
        try {
            sock.setKeepAlive(on);
        } catch (SocketException e) {
            System.out.println("WARNING: "+e.getMessage());
        }
    }

    boolean close () {
        try {
            sock.close();
        } catch (IOException e) {
            System.out.println("WARNING: "+e.getMessage());
        }
        return sock.isClosed();
    }

    void sendPacket (String field, int data) {
        try {
            sendThread thread = new sendThread(field, data, sock.getOutputStream());
            thread.start();
            thread.join();//thread.stop();
        } catch (IOException e) { //getOutputStream
            System.out.println("WARNING: getOutputStream(): "+e.getMessage());
        } catch (InterruptedException e) {
            System.out.println("WARNING: thread: "+e.getMessage());
        }
    }

    static void assertErr(boolean cond, String errmsg) {
        if (!cond) {
            System.out.println(errmsg);
        }
    }

    static void assertFail(boolean cond, String errmsg) {
        if (!cond) {
            System.out.println(errmsg);
            System.out.println("fatal.\n");
            System.exit(-1);
        }
    }
    
    
    socketSender(int localPort, int remotePort) {
        this.dest_port = remotePort;
        this.local_port = localPort;
        this.sock = new Socket();
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


    public static void main(String args[]) {
        Scanner scan = new Scanner(System.in);
        int remotePort;
        System.out.print("enter remote host address >> ");
        while (!scan.hasNextInt()) {
            System.out.print("enter remote host address >> ");
        }
        remotePort = scan.nextInt();
        socketSender ss = new socketSender (12344, remotePort, "192.168.0.107");
        assertErr(ss.bindLocal(), "ERROR: socket failed to bind to local endpoint.\n");
        assertFail(ss.connect(), "ERROR: socket failed to connect to remote endpoint.\n");
        ss.setKeepAlive(true);
        ss.setReuseAddress(true);
        String field = "field";
        int data = 0;
        while (data < 1000) {
            System.out.println("Active threads: "+Thread.activeCount());
            ss.sendPacket(field, data);
            data+=100;
            try {
                Thread.currentThread().sleep(5000); //ms
            } catch (InterruptedException e) {
                System.out.println("WARNING: "+e.getMessage());
            }
        }
        assertFail(ss.close(), "ERROR: socket failed to close.\n");
    }
}
