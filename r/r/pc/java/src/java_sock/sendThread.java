import java.io.*;

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


