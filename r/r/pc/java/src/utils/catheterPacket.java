import java.util.*;
import java.io.*;

class catheterPacket {
    private int delayMS;
    private catheterChannelCmd[] channels;
    private boolean[] include;
    private int nchannels;

    public catheterPacket() {
        this(utils.NCHANNELS);
    }

    public catheterPacket(int nchannels) {
        this.nchannels = nchannels;
        channels = new catheterChannelCmd[nchannels];
        for (int i=0; i < nchannels; i++)
            channels[i] = new catheterChannelCmd();
        include = new boolean[nchannels];
    }

    public static ArrayList<catheterPacket> loadPlayfile(File file, ArrayList<catheterPacket> packets) {
        int extensionIndex = file.getName().lastIndexOf('.');
        String ext = file.getName().substring(extensionIndex + 1);
        if (ext.equals("play")) {
            return loadPlayfile_play(file, packets);
        } else if (ext.equals("csv") || ext.equals("xls")) {
            return loadPlayfile_csv(file, packets);
        }
        return packets;
    }

    private static ArrayList<catheterPacket> loadPlayfile_play(File file, ArrayList<catheterPacket> packets) {
        boolean debug = true;
        int CHAN_INDEX = 0;
        int CURRENT_INDEX = 1;
        int DIR_INDEX = 2;
        int DELAY_INDEX = 3;

        int num_channels = utils.NCHANNELS; //default value
        if (packets.size() > 0)
            num_channels = packets.get(0).getNumChannels();
        packets.clear();

        try {

            catheterPacket p = new catheterPacket(num_channels);

            BufferedReader br = new BufferedReader(new FileReader(file));
            String line;
            while ((line=br.readLine()) != null) {
                if (debug) System.out.println("next line: " + line);

                if (line.length() < 7) continue; //min length: x,x,x,x
                
                if (line.charAt(0) == '#') continue; //comment

                line = line.replaceAll(" ","");

                String[] fields = line.split(",");
                if (fields.length != 4) continue; //bad line
                if (debug) System.out.println("fields:"+fields[0]+":"+fields[1]+":"+fields[2]+":"+fields[3]);

                try {
                    int chan = Integer.parseInt(fields[CHAN_INDEX]);
                    if (chan < 0 || chan > num_channels) continue;
                    if (debug) System.out.println("\tchannel: " + chan);

                    int direction = Integer.parseInt(fields[DIR_INDEX]);
                    String dirStr = (direction > 0 ? "positive" : "negative");
                    if (debug) System.out.println("\tdirection: " + dirStr);
                    
                    int dacRes = Integer.parseInt(fields[CURRENT_INDEX]);
                    if (debug) System.out.println("\tDac Resolution: " + dacRes);

                    boolean poll = false;
                   
                    for (int i=0; i<num_channels; i++) {
                        if (chan==0 || i==(chan-1)) {
                            p.includeChannel(i);
                            p.setChannelDirection(i, dirStr);
                            p.setChannelCurrentRes(i, dacRes);
                        }
                    }
                    /*boolean enable = (dacRes != utils.OFF_CurrentRes);
                    boolean update = false;
                    if (enable || (packets.size() > 0 && 
                      (!packets.get(packets.size()-2).isChannelIncluded(chan) || 
                      (!p.getChannelDirection(chan).equals(packets.get(packets.size()-2).getChannelDirection(chan)))))) {
                        update = true; //change in direction
                    }*/ //do not actually need to store these => let cpp program infer this
                    
                    int cmdDelay = Integer.parseInt(fields[DELAY_INDEX]);
                    if (cmdDelay < 0) continue;

                    if (debug) System.out.println("\tdelay: " + cmdDelay);
                    
                    if (cmdDelay > 0) {
                        p.delayMS = cmdDelay;
                        packets.add(p);
                        if (debug) System.out.println("\nADDING PACKET:\n"+p.present()+"\n");
                        p = new catheterPacket(num_channels);
                    }
                    
                } catch (NumberFormatException e) {
                    if (debug) System.out.println("NumberFormatException");
                    continue;
                }
            }
            br.close();
        } catch (IOException e) {
            utils.exceptMsg("",e);
        }
        if (debug) {
            for (catheterPacket p : packets)
                System.out.println(p.present());
        }
        return packets;
    }

    private static ArrayList<catheterPacket> loadPlayfile_csv(File file, ArrayList<catheterPacket> packets) {
        boolean debug = true;
        return packets;
    }

    public int getNumChannels() {
        return nchannels;
    }
    
    public boolean isChannelIncluded(int channel) {
        return include[channel];
    }
    public void includeChannel(int channel) {
        include[channel] = true;
    }
    public void disableChannel(int channel) {
        include[channel] = false;
    }
    
    public catheterChannelCmd[] getAllChannels() { return channels; }
    public catheterChannelCmd[] getIncludedChannels() {
        int sum=0;
        for (int i=0; i < channels.length; i++)
            if (include[i]) sum++;
        catheterChannelCmd[] included = new catheterChannelCmd[sum];
        for (int i=0; i< channels.length; i++) {
            if (include[i]) {
                included[sum-1] = channels[i];
                sum--;
            }
        }
        return included;
    }

    public String present() {
        StringBuilder sb = new StringBuilder(); 
        for (int i=0; i<channels.length; i++) {
            if (include[i]) {
                sb.append("channel "+ i +":");
                sb.append("\tpoll: "+ boolStr(channels[i].getPoll()));
                sb.append("\tdir: "+ channels[i].getDirection());
                sb.append("\tcurrent: "+ channels[i].getCurrentMA() +"MA\n");
            }
        }
        sb.append("packet delay: "+ delayMS +"ms\n");
        return sb.toString();
    }
    private static String boolStr(boolean b) {
        if (b) return "yes";
        else return "no";
    }

    public void setDelayMS(int delay) { this.delayMS = delay; }
    public int getDelayMS () { return delayMS; }
    
    public void setChannelCurrentMA(int channel, double currentMA) {
        include[channel] = true;
        channels[channel].setCurrentMA(currentMA);
    }
    public void setChannelCurrentRes(int channel, int res) {
        include[channel] = true;
        channels[channel].setCurrentRes(res);
    }
    public double resetChannelCurrentMA(int channel) {
        channels[channel].setCurrentMA(0.0);
        return channels[channel].getCurrentMA();
    }
    public double getChannelCurrentMA(int channel) {
        return channels[channel].getCurrentMA();
    }
    public int getChannelCurrentRes(int channel) {
        return channels[channel].getCurrentRes();
    }
    
    public void setChannelDirection(int channel, String dir) {
        include[channel] = true;
        if (dir.equals("pos") || dir.equals("positive"))
            channels[channel].setDirection("pos");
        else if (dir.equals("neg") || dir.equals("negative"))
            channels[channel].setDirection("neg");
    }
    public String getChannelDirection (int channel) {
        return channels[channel].getDirection();
    }

    public void setChannelPoll(int channel, boolean poll) {
        include[channel] = true;
        channels[channel].setPoll(poll);
    }
    public boolean getChannelPoll (int channel) {
        return channels[channel].getPoll();
    }

    public void clearChannel(int channel) {
        include[channel] = false;
        channels[channel].clear();
    }

    public void clear() {
        for (int i=0; i < channels.length; i++)
            clearChannel(i);
    }

    public void setGlobalReset() {
        for (int i=0; i < channels.length; i++) {
            include[i] = true;
            channels[i].configureReset();
        }
    }

    public void setGlobal() {
        for (int i=0; i < channels.length; i++)
            include[i] = true;
    }



    class catheterChannelCmd {
        private boolean pol;
        private String dir;
        private int currentRes;

        catheterChannelCmd () {
            dir = "pos";
            currentRes=4095;
        }
        
        int currentFunction (double ma) {
            int res = (int)(utils.maxCurrentRes*ma);
            return res;
        }
        double reverseCurrentFunction (int res) {
            double ma = (double)(res)/utils.maxCurrentRes;
            return ma;
        }

        void setCurrentMA(double current) {
            currentRes = currentFunction(current);
        }
        double getCurrentMA() {
            return reverseCurrentFunction(currentRes);
        }
        void setCurrentRes(int res) {
            if (res >= 0 && res <= utils.maxCurrentRes)
                currentRes = res; 
        }
        int getCurrentRes() {return currentRes;}
        
        void setDirection(String dir) {
            this.dir = dir;
        }
        String getDirection() {return dir;}

        void setPoll(boolean pol) {
            this.pol = pol;
        }
        boolean getPoll() {return pol;}
        
        void clear() {
            pol=false;
            dir="pos";
            currentRes=4095;
        }

        void configureReset() {
            pol=false;
            dir="pos";
            currentRes=4095;
        }
    }
}
