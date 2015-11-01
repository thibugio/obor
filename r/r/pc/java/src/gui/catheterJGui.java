import javax.swing.JFrame;
import javax.swing.JButton; 
import javax.swing.BoxLayout;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JTextArea;
import javax.swing.text.*;
import javax.swing.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;

public class catheterJGui implements ActionListener{
  
    private final int INC_OPT = 0;
    private final int POL_OPT = 1;
    private final int DIR_OPT = 2;
    private final int CUR_OPT = 3;
    
    private final short NCHANNELS=6;
    // window for the application 
    private JFrame guiframe;
    //buttons
    private static JButton[][] chanOptButtons;
    private static JButton connectSocketButton;
    private static JButton connectSerialButton;
    private static JButton uploadPacketsButton;
    private static JButton sendPacketsButton;
    private static JButton sendResetButton;
    private static JButton clearAllButton;
    private static JButton clearChannelButton;
    private static JButton packetPreviewButton;
    private static JButton packetPreviewNextButton;
    private static JButton packetPreviewDoneButton;
    private static JButton packetPreviewBytesButton;
    private static JButton packetSourceButton;
    //text fields 
    private static JTextField[] chanCurrentTFs;
    private static JTextArea statusText;
    private static JTextField remotePortTF;
    private static JTextField remoteAddressTF;
    
    private Color colorFalse;
    private Color colorTrue;

    private ArrayList<catheterPacket> user_packets;
    private ArrayList<catheterPacket> file_packets;
    private int user_pnum;
    private int file_pnum;
    private int packetPreviewNum;
    private boolean sourceIsFile;

    private socketSender sock;

    private static JButton[] testbuttons;
    
    public catheterJGui(){
        
        sock = new socketSender (12345, "192.168.0.107");
        sock.setReuseAddress(true);

        user_packets = new ArrayList<catheterPacket>();
        file_packets = new ArrayList<catheterPacket>();
        sourceIsFile = false;
        
        guiframe = new JFrame("Catheter GUI");

        JPanel channelsPanel = new JPanel(); //container
        BoxLayout channelsBox = new BoxLayout(channelsPanel, BoxLayout.Y_AXIS); //layout manager

        JPanel[] channelPanels = new JPanel[NCHANNELS];
        BoxLayout channelBoxes[] = new BoxLayout[NCHANNELS];
        
        for (int i=0; i < NCHANNELS; i++) {
            channelPanels[i] = new JPanel();
            channelBoxes[i] = new BoxLayout(channelPanels[i], BoxLayout.X_AXIS);
        }
        
        chanOptButtons = new JButton[NCHANNELS][4];
        colorFalse = Color.lightGray;
        colorTrue = Color.gray;

        chanCurrentTFs = new JTextField[NCHANNELS];

        for(int i = 0; i < NCHANNELS; ++i){
            channelPanels[i].add(new JLabel("Channel "+i+": "));            

            chanOptButtons[i][INC_OPT] = new JButton("INCLUDE");
            chanOptButtons[i][INC_OPT].addActionListener(this);
            chanOptButtons[i][INC_OPT].setBackground(colorFalse);
            channelPanels[i].add(chanOptButtons[i][INC_OPT]);

            chanOptButtons[i][POL_OPT] = new JButton("POLL");
            chanOptButtons[i][POL_OPT].addActionListener(this);
            chanOptButtons[i][POL_OPT].setBackground(colorFalse);
            channelPanels[i].add(chanOptButtons[i][POL_OPT]);

            chanOptButtons[i][DIR_OPT] = new JButton("DIR POS");
            chanOptButtons[i][DIR_OPT].addActionListener(this);
            chanOptButtons[i][DIR_OPT].setBackground(colorFalse);
            channelPanels[i].add(chanOptButtons[i][DIR_OPT]);

            chanOptButtons[i][CUR_OPT] = new JButton("UPDATE CURRENT");
            chanOptButtons[i][CUR_OPT].addActionListener(this);
            chanOptButtons[i][CUR_OPT].setBackground(colorFalse);
            channelPanels[i].add(chanOptButtons[i][CUR_OPT]);

            chanCurrentTFs[i] = new JTextField("0.0", 8);
            chanCurrentTFs[i].addActionListener(this);
            channelPanels[i].add(chanCurrentTFs[i]);

            channelsPanel.add(channelPanels[i]);
        }
        
        JPanel packetControlPanel = new JPanel();
        BoxLayout packetControlBox = new BoxLayout(packetControlPanel, BoxLayout.X_AXIS);
        
        JPanel connectPanel = new JPanel();
        BoxLayout connectBox = new BoxLayout(connectPanel, BoxLayout.X_AXIS);
        
        JPanel sendPanel = new JPanel();
        BoxLayout sendBox = new BoxLayout(sendPanel, BoxLayout.X_AXIS);
        
        JPanel previewButtonPanel = new JPanel();
        BoxLayout previewButtonBox = new BoxLayout(previewButtonPanel, BoxLayout.X_AXIS);
        
        JPanel statusTextPanel = new JPanel();       
        BoxLayout statusTextBox = new BoxLayout(statusTextPanel, BoxLayout.X_AXIS);
       
         
        remotePortTF = new JTextField("12345");
        remoteAddressTF = new JTextField("192.168.0.107");
        
        connectSocketButton = new JButton("CONNECT REMOTE");
        connectSocketButton.setBackground(colorFalse);
        connectSocketButton.addActionListener(this);
        connectSerialButton = new JButton("CONNECT SERIAL");
        connectSerialButton.setBackground(colorFalse);
        connectSerialButton.addActionListener(this);
        uploadPacketsButton = new JButton("UPLOAD PACKETS");
        uploadPacketsButton.setBackground(colorFalse);
        uploadPacketsButton.addActionListener(this);
       
        connectPanel.add(new JLabel("remote port"));
        connectPanel.add(remotePortTF);
        connectPanel.add(new JLabel("remote host"));
        connectPanel.add(remoteAddressTF);
        connectPanel.add(connectSocketButton); 
        connectPanel.add(connectSerialButton);
        connectPanel.add(uploadPacketsButton);
        
        
        sendPacketsButton = new JButton("SEND PACKETS");
        sendPacketsButton.setBackground(colorFalse);
        sendPacketsButton.addActionListener(this);
        sendResetButton = new JButton("SEND GLOBAL RESET");
        sendResetButton.setBackground(colorFalse);
        sendResetButton.addActionListener(this);
        packetPreviewBytesButton = new JButton("SHOW BYTES");
        packetPreviewBytesButton.setBackground(colorFalse);
        packetPreviewBytesButton.addActionListener(this);
        packetPreviewBytesButton.setVisible(false);
        
        sendPanel.add(sendPacketsButton);
        sendPanel.add(sendResetButton);
        sendPanel.add(packetPreviewBytesButton);
       

        clearChannelButton = new JButton("CLEAR CHANNEL");
        clearChannelButton.setBackground(colorFalse);
        clearChannelButton.addActionListener(this);
        clearAllButton = new JButton("CLEAR ALL PACKETS");
        clearAllButton.setBackground(colorFalse);
        clearAllButton.addActionListener(this);
        packetSourceButton = new JButton("PACKET SOURCE: USER");
        packetSourceButton.setBackground(colorFalse);
        packetSourceButton.addActionListener(this);
        
        packetControlPanel.add(clearChannelButton);
        packetControlPanel.add(clearAllButton);
        packetControlPanel.add(packetSourceButton);


        packetPreviewButton = new JButton("PREVIEW PACKETS");
        packetPreviewButton.setBackground(colorFalse);
        packetPreviewButton.addActionListener(this);
        packetPreviewNextButton = new JButton("NEXT");
        packetPreviewNextButton.setBackground(colorTrue);
        packetPreviewNextButton.addActionListener(this);
        packetPreviewNextButton.setVisible(false);
        packetPreviewDoneButton = new JButton("DONE");
        packetPreviewDoneButton.setBackground(colorTrue);
        packetPreviewDoneButton.addActionListener(this);
        packetPreviewDoneButton.setVisible(false);

        previewButtonPanel.add(packetPreviewButton);
        previewButtonPanel.add(packetPreviewNextButton);
        previewButtonPanel.add(packetPreviewDoneButton);
        

        statusText = new JTextArea("Disconnected from remote.\n", 12, 64);
        statusText.append("Disconnected from serial controller.\n");
        statusText.setBackground(guiframe.getBackground());
        statusText.setEditable(false);
        
        statusTextPanel.add(statusText);

        
        JPanel bottomControlPanel = new JPanel();
        BoxLayout bottomControlBox = new BoxLayout(bottomControlPanel, BoxLayout.Y_AXIS);
        
        bottomControlPanel.add(packetControlPanel);
        bottomControlPanel.add(connectPanel);
        bottomControlPanel.add(sendPanel);
        bottomControlPanel.add(statusTextPanel);
        bottomControlPanel.add(previewButtonPanel);

        JPanel[] testpanels = new JPanel[5];
        BoxLayout[] testboxes = new BoxLayout[5];
        testbuttons = new JButton[5];
        for (int i=0; i<5; i++) {
            testbuttons[i] = new JButton("test"+i);
            testbuttons[i].addActionListener(this);
            testpanels[i] = new JPanel();
            testpanels[i].add(testbuttons[i]); //testpanels[0] contains no panels.
            if (i>0) {
                testpanels[i].add(testpanels[0]); //testpanels[1] contains tp[0]
            }
            if (i>1) {
                testpanels[i].add(testpanels[1]); //testpanels[2] contains tp[0] and tp[1]
            }
            if (i>2) {
                testpanels[i].add(testpanels[2]); //tp[3] contains tp[0], tp[1], and tp[2]
            }
            if (i>3){ 
                testpanels[i].add(testpanels[3]); //tp[4] contains tps 0-3
            }
            testboxes[i] = new BoxLayout(testpanels[i], BoxLayout.X_AXIS);
        }


        guiframe.getContentPane().setLayout(new BoxLayout(guiframe.getContentPane(), BoxLayout.Y_AXIS));
        guiframe.getContentPane().add(channelsPanel);
        guiframe.getContentPane().add(bottomControlPanel);
        guiframe.getContentPane().add(testpanels[4]);
        guiframe.setSize(800,500);
        guiframe.setVisible(true);
    }

    void changeStatusLine(String msg, int line) {
        try {
            statusText.replaceRange(msg,statusText.getLineStartOffset(line),statusText.getLineEndOffset(line));
        } catch (BadLocationException e) {
            utils.exceptMsg("", e);
        }
    }

    void errorDialog (String msg) {
        JOptionPane.showMessageDialog(null, msg, "WARNING!", JOptionPane.INFORMATION_MESSAGE);
    }
    
    void remoteConnect() {
        sock.setRemotePort(Integer.parseInt(remotePortTF.getText()));
        sock.setRemoteAddress(remoteAddressTF.getText());
        if (connectSocketButton.getBackground().equals(colorFalse)) {
            if (!sock.connect())
                changeStatusLine("Could not connect to remote server.\n", 0);
            else {
                connectSocketButton.setBackground(colorTrue);
                connectSocketButton.setText("DISCONNECT REMOTE");
                changeStatusLine("Connected to remote.\n", 0);
            }
        } else {
            if (!sock.close())
                changeStatusLine("Failed to close remote server.\n", 0);
            else {
                connectSocketButton.setBackground(colorFalse);
                connectSocketButton.setText("CONNECT REMOTE");
                changeStatusLine("Disconnected from remote.\n", 0);
            }
        }
    }

    void serialConnect() {
        //byte[] buf = sock.sendPacket1AndReturn(utils.CONNECT, 0);
        //boolean success = buf[buf.length-1] > 0;
        boolean success = false;
        if (!success) {
            changeStatusLine("Could not connect to serial controller.\n", 1);
            connectSerialButton.setText("CONNECT SERIAL");
        } else {
            connectSerialButton.setBackground(colorTrue);
            connectSerialButton.setText("DISCONNECT SERIAL");
            changeStatusLine("Connected to serial controller.\n", 1);
        }
    }

    void sendChannelUpdate(int channel, String cmd, int data) {
        if (sourceIsFile)
            sendChannelUpdate(channel, cmd, data, file_pnum);
        else 
            sendChannelUpdate(channel, cmd, data, user_pnum);
    }
    void sendChannelUpdate(int channel, String cmd, int data, int pnum) {
        sock.sendPacket1(utils.PACKET_NUM, pnum);
        sock.sendPacket1(utils.CHANNEL_NUM, channel);
        sock.sendPacket1(cmd, data);
    }

    void checkNewPacket() {
        if (sourceIsFile)
            checkNewPacket(file_packets, file_pnum);
        else 
            checkNewPacket(user_packets, user_pnum);
    }
    void checkNewPacket(ArrayList<catheterPacket> packets, int pnum) {
        if (packets.isEmpty()) {
            packets.add(new catheterPacket(NCHANNELS));
        }
    }
    
    boolean uploadPackets() {
        if (sourceIsFile)
            return uploadPackets(file_packets);
        else
            return uploadPackets(user_packets);
    }
    boolean uploadPackets(ArrayList<catheterPacket> packets) {
        for (int i=0; i<packets.size(); i++) {
            sock.sendPacket1(utils.NEW_PACKET, 0);
            sock.sendPacket1(utils.PACKET_NUM, i);
            for (int j=0; j<NCHANNELS; j++) {
                if (packets.get(i).isChannelIncluded(j)) {
                    sock.sendPacket1(utils.NEW_CHANNEL, 0);
                    sock.sendPacket1(utils.CHANNEL_NUM, j);
                    sock.sendPacket1(utils.POLL, packets.get(i).getChannelPoll(j));
                    sock.sendPacket1(utils.DIRECTION, packets.get(i).getChannelDirection(j));
                    sock.sendPacket1(utils.CURRENT, packets.get(i).getChannelCurrentRes(j));
                    sock.sendPacket1(utils.DELAY, packets.get(i).getDelayMS());
                }
            }
        }
        packetPreviewBytesButton.setBackground(colorFalse);
        packetPreviewBytesButton.setVisible(true);
        return true; 
    }

    void previewPacketBytes() {
        packetPreviewBytesButton.setBackground(colorTrue);
    }

    void sendPackets() {
        sock.sendPacket1(utils.SEND_PACKETS, 0);
        packetPreviewBytesButton.setBackground(colorFalse);
        packetPreviewBytesButton.setVisible(false);
    }

    void sendGlobalReset() {
        sock.sendPacket1(utils.SEND_GLOBAL_RESET, 0);
    }

    void clearAllPackets() {
        if (sourceIsFile)
            clearAllPackets(file_packets);
        else 
            clearAllPackets(user_packets);
    }
    void clearAllPackets(ArrayList<catheterPacket> packets) {
        sock.sendPacket1(utils.CLEAR_ALL_PACKETS, 0);
        packets.clear();
    }

    void clearChannel(int channel) {
        if (sourceIsFile)
            clearChannel(channel, file_packets, file_pnum);
        else 
            clearChannel(channel, user_packets, user_pnum);
    }
    void clearChannel(int channel, ArrayList<catheterPacket> packets, int pnum) {
        sock.sendPacket1(utils.DEL_CHANNEL, channel);
        packets.get(pnum).clearChannel(channel);
    }

    void clearPacketPreviewText() {
        try {
            statusText.replaceRange("", statusText.getLineStartOffset(2),     
                                statusText.getLineEndOffset(statusText.getLineCount()-1));
        } catch (BadLocationException e) {
            utils.exceptMsg("", e);
        }
    }
    
    void previewNextPacket() {
        if (sourceIsFile)
            previewNextPacket(file_packets);
        else 
            previewNextPacket(user_packets);
    }
    void previewNextPacket(ArrayList<catheterPacket> packets) {
        System.out.println("packetPrevNum: "+packetPreviewNum+"\tSize of packets: "+packets.size());
        if (packets.isEmpty()) {
            packetPreviewNextButton.setBackground(colorFalse);
        } else {
            if (packetPreviewNum < packets.size()-1)
                packetPreviewNextButton.setBackground(colorTrue);
            else
                packetPreviewNextButton.setBackground(colorFalse);
            clearPacketPreviewText();
            statusText.append(packets.get(packetPreviewNum).present()+"\n");
            packetPreviewNum = (packetPreviewNum + 1) % packets.size();
        }
    }

    void endPreview() {
        packetPreviewNextButton.setVisible(false);
        packetPreviewDoneButton.setVisible(false);
        packetPreviewNum = 0;
        clearPacketPreviewText();
    }

    void beginPreview() {
        packetPreviewNextButton.setVisible(true);
        packetPreviewDoneButton.setVisible(true);
        packetPreviewNum = 0;
        previewNextPacket();
    }

    void loadPacketDisplay(catheterPacket p) {
        for (int i=0; i<NCHANNELS; i++) {
            if (p.isChannelIncluded(i)) {
                if (p.getChannelPoll(i))
                    chanOptButtons[i][0].setBackground(colorTrue);
                else
                    chanOptButtons[i][0].setBackground(colorFalse);

                if (p.getChannelDirection(i).equals("positive"))
                    chanOptButtons[i][0].setText("DIR_POS");
                else
                    chanOptButtons[i][0].setText("DIR_NEG");

                chanCurrentTFs[i].setText((new Double (p.getChannelCurrentMA(i))).toString());
            }
        }
    }

    void reloadPacketDisplay() {
        if (sourceIsFile) {
            loadPacketDisplay (file_packets.get(file_pnum));
        } else {
            loadPacketDisplay (user_packets.get(user_pnum));
        }
    }

    void togglePacketSource () {
        if (packetSourceButton.getText().equals("PACKET SOURCE: USER")) {
            packetSourceButton.setText("PACKET SOURCE: FILE");
            sourceIsFile = true;
        } else {
            packetSourceButton.setText("PACKET SOURCE: USER");
            sourceIsFile = false;
        }
        reloadPacketDisplay();
    }

    void includeChannel(int channel, ArrayList<catheterPacket> packets, int pnum) {
        packets.get(pnum).includeChannel(channel);
        chanOptButtons[channel][INC_OPT].setBackground(colorTrue);
    }
    
    void updateChannelCurrent(int channel) {
        if (sourceIsFile)
            updateChannelCurrent(channel, file_packets, file_pnum);
        else 
            updateChannelCurrent(channel, user_packets, user_pnum);
    }
    void updateChannelCurrent(int channel, ArrayList<catheterPacket> packets, int pnum) {
        checkNewPacket();
        String current;
        double newcurrent;
        try {
            current = chanCurrentTFs[channel].getText(); 
            if (current == null || current.equals("")) {
                newcurrent = packets.get(pnum).resetChannelCurrentMA(channel);            
                chanCurrentTFs[channel].setText((new Double(newcurrent)).toString());
            } else {
                try {
                    newcurrent = Double.valueOf(current);
                    if (newcurrent < 0) throw new NumberFormatException();
                    packets.get(pnum).setChannelCurrentMA(channel, newcurrent);
                } catch (NumberFormatException nfe) {
                    newcurrent = packets.get(pnum).resetChannelCurrentMA(channel);            
                    chanCurrentTFs[channel].setText((new Double(newcurrent)).toString());
                }
            }
        } catch (NullPointerException e) {
            newcurrent = packets.get(pnum).resetChannelCurrentMA(channel);            
            chanCurrentTFs[channel].setText((new Double(newcurrent)).toString());
        }
    }

    void updateChannelOption(int channel, int opt) {
        if (sourceIsFile)
            updateChannelOption(channel, opt, file_packets, file_pnum);
        else 
            updateChannelOption(channel, opt, user_packets, user_pnum);
    }
    void updateChannelOption(int channel, int opt, ArrayList<catheterPacket> packets, int pnum) {
        checkNewPacket();
        switch (opt) {
            case INC_OPT:
                if (!packets.get(pnum).isChannelIncluded(channel)) {
                    includeChannel(channel, packets, pnum);
                } else {
                    packets.get(pnum).disableChannel(channel);
                    chanOptButtons[channel][opt].setBackground(colorFalse);
                }
                break;
            case POL_OPT: 
                if (packets.get(pnum).getChannelPoll(channel)) {
                    packets.get(pnum).setChannelPoll(channel, false);
                    chanOptButtons[channel][opt].setBackground(colorFalse);
                } else {
                    includeChannel(channel, packets, pnum);
                    packets.get(pnum).setChannelPoll(channel, true);
                    chanOptButtons[channel][opt].setBackground(colorTrue);
                }
                break;
            case DIR_OPT: 
                includeChannel(channel, packets, pnum);
                if (packets.get(pnum).getChannelDirection(channel).equals("positive")) {
                    packets.get(pnum).setChannelDirection(channel, "negative");
                    chanOptButtons[channel][opt].setText("DIR_NEG");
                } else {
                    packets.get(pnum).setChannelDirection(channel, "positive");
                    chanOptButtons[channel][opt].setText("DIR_POS");
                }
                break;
            case CUR_OPT: //apply current  
                if (chanOptButtons[channel][opt].getBackground().equals(colorFalse)) {
                    includeChannel(channel, packets, pnum);
                    updateChannelCurrent(channel);
                    chanOptButtons[channel][opt].setBackground(colorTrue);
                } else { //un-apply current
                    double newcurrent = packets.get(pnum).resetChannelCurrentMA(channel);            
                    chanCurrentTFs[channel].setText((new Double(newcurrent)).toString());
                    chanOptButtons[channel][opt].setBackground(colorFalse);
                }
                break;
        }
    }

    void printNumListeners (JComponent e, String action) {
        EventListener[] listeners = e.getListeners(catheterJGui.class);
        System.out.println("event "+e.toString()+" has "+listeners.length+" listeners.");
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        for (int i=0; i<5; i++) {
            if (e.getSource().equals(testbuttons[i])) {
                printNumListeners(testbuttons[i], "");
                break;
            }
        }
        for (int i=0; i < NCHANNELS; ++i) {
            if (e.getSource().equals(chanOptButtons[i][0])) {
                printNumListeners(chanOptButtons[i][0], e.getActionCommand());
                updateChannelOption(i,0);
            } else if (e.getSource().equals(chanOptButtons[i][1])) {
                updateChannelOption(i,1);
            } else if (e.getSource().equals(chanOptButtons[i][2])) {
                updateChannelOption(i,2);
            } else if (e.getSource().equals(chanCurrentTFs[i])) {
                printNumListeners(chanCurrentTFs[i], e.getActionCommand());
                updateChannelCurrent(i);
            } else if (e.getSource().equals(connectSocketButton)) {
                remoteConnect();
            } else if (e.getSource().equals(connectSerialButton)) {
                serialConnect();
            } else if (e.getSource().equals(sendPacketsButton)) {
                sendPackets();
            } else if (e.getSource().equals(sendResetButton)) {
                sendGlobalReset();
            } else if (e.getSource().equals(clearAllButton)) {
                clearAllPackets();
            } else if (e.getSource().equals(clearChannelButton)) {
                clearChannel(i);
            } else if (e.getSource().equals(packetPreviewButton)) {
                printNumListeners(packetPreviewButton, e.getActionCommand());
                beginPreview();
            } else if (e.getSource().equals(packetPreviewNextButton) && packetPreviewNextButton.isVisible()) {
                previewNextPacket();
            } else if (e.getSource().equals(packetPreviewDoneButton) && packetPreviewDoneButton.isVisible()) {
                endPreview();
            } else if (e.getSource().equals(packetPreviewBytesButton) &&packetPreviewBytesButton.isVisible()){
                previewPacketBytes();
            } else if (e.getSource().equals(packetSourceButton)) {
                togglePacketSource();
            }
        }
    }
    
    
}  
