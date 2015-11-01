import javax.swing.JFrame;
import javax.swing.JButton; 
import javax.swing.BoxLayout;
import javax.swing.JDialog;
import javax.swing.JOptionPane;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.Color;
import javax.swing.JPanel;
import javax.swing.JLabel;
import javax.swing.JTextField;
import javax.swing.JTextArea;
import javax.swing.JFileChooser;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.JComponent;
import java.text.DecimalFormat;
import java.math.RoundingMode;
import javax.swing.text.*;
import java.awt.event.*;
import java.awt.*;
import java.util.*;
import java.io.*;

public class cjg {
  
    private final int INC_OPT = 0;
    private final int POL_OPT = 1;
    private final int DIR_OPT = 2;
    private final int CUR_OPT = 3;
    
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
    private static JButton packetPreviewButton;
    private static JButton packetPreviewNextButton;
    private static JButton packetPreviewDoneButton;
    private static JButton packetPreviewBytesButton;
    private static JButton packetSourceButton;
    private static JButton addPacketButton;
    private static JButton removePacketButton;
    private static JButton pickPlayfileButton;
    
    //text fields 
    private static JTextField[] chanCurrentTFs;
    private static JTextArea statusText;
    private static JTextField remotePortTF;
    private static JTextField remoteAddressTF;
    private static JLabel pnumLabel;
    
    private static JFileChooser playfilePicker;
    private static JLabel playfilePathLabel;
    
    private Color colorFalse;
    private Color colorTrue;

    private ArrayList<catheterPacket> user_packets;
    private ArrayList<catheterPacket> file_packets;
    private int user_pnum;
    private int file_pnum;
    private int packetPreviewNum;
    private boolean sourceIsFile;

    private socketSender sock;

    private File playfile;

    private DecimalFormat decformatter;    


    public cjg(){
        
        decformatter = new DecimalFormat("#.####");
        decformatter.setRoundingMode(RoundingMode.CEILING);

        sock = new socketSender (12345, "192.168.0.107");
        sock.setReuseAddress(true);

        user_packets = new ArrayList<catheterPacket>();
        file_packets = new ArrayList<catheterPacket>();
        sourceIsFile = false;
        playfile = null;

        ChanIncListener inc_lis = new ChanIncListener();
        ChanPollListener poll_lis = new ChanPollListener();
        ChanDirListener dir_lis = new ChanDirListener();
        ChanCurrentListener current_lis = new ChanCurrentListener();
        PreviewListener preview_lis = new PreviewListener();
        PacketsListener packet_lis = new PacketsListener();
        FilesListener file_lis = new FilesListener();
        ConnectionListener conn_lis = new ConnectionListener();

        colorFalse = Color.lightGray;
        colorTrue = Color.gray;

        
        guiframe = new JFrame("Catheter GUI");

        JPanel channelsPanel = new JPanel(); //container
        BoxLayout channelsBox = new BoxLayout(channelsPanel, BoxLayout.Y_AXIS); //layout manager

        JPanel[] channelPanels = new JPanel[utils.NCHANNELS];
        BoxLayout channelBoxes[] = new BoxLayout[utils.NCHANNELS];
        
        for (int i=0; i < utils.NCHANNELS; i++) {
            channelPanels[i] = new JPanel();
            channelBoxes[i] = new BoxLayout(channelPanels[i], BoxLayout.X_AXIS);
        }
        
        chanOptButtons = new JButton[utils.NCHANNELS][4];

        chanCurrentTFs = new JTextField[utils.NCHANNELS];


        for(int i = 0; i < utils.NCHANNELS; ++i){
            channelPanels[i].add(new JLabel("Channel "+i+": "));            

            chanOptButtons[i][INC_OPT] = new JButton("INCLUDE");
            chanOptButtons[i][INC_OPT].addActionListener(inc_lis);
            chanOptButtons[i][INC_OPT].setBackground(colorFalse);
            channelPanels[i].add(chanOptButtons[i][INC_OPT]);

            chanOptButtons[i][POL_OPT] = new JButton("POLL");
            chanOptButtons[i][POL_OPT].addActionListener(poll_lis);
            chanOptButtons[i][POL_OPT].setBackground(colorFalse);
            channelPanels[i].add(chanOptButtons[i][POL_OPT]);

            chanOptButtons[i][DIR_OPT] = new JButton("DIR_POS");
            chanOptButtons[i][DIR_OPT].addActionListener(dir_lis);
            chanOptButtons[i][DIR_OPT].setBackground(colorFalse);
            channelPanels[i].add(chanOptButtons[i][DIR_OPT]);

            chanOptButtons[i][CUR_OPT] = new JButton("UPDATE CURRENT");
            chanOptButtons[i][CUR_OPT].addActionListener(current_lis);
            chanOptButtons[i][CUR_OPT].setBackground(colorFalse);
            channelPanels[i].add(chanOptButtons[i][CUR_OPT]);

            chanCurrentTFs[i] = new JTextField("0.0", 8);
            chanCurrentTFs[i].addActionListener(current_lis);
            channelPanels[i].add(chanCurrentTFs[i]);

            channelsPanel.add(channelPanels[i]);
        }

        JPanel fileIOPanel = new JPanel();
        BoxLayout fileIOBox = new BoxLayout(fileIOPanel, BoxLayout.X_AXIS);
        
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
        setColorFalse(connectSocketButton);
        connectSocketButton.addActionListener(conn_lis);
        connectSerialButton = new JButton("CONNECT SERIAL");
        setColorFalse(connectSerialButton);
        connectSerialButton.addActionListener(conn_lis);
        uploadPacketsButton = new JButton("UPLOAD PACKETS");
        setColorFalse(uploadPacketsButton);
        uploadPacketsButton.addActionListener(conn_lis);
        
       
        connectPanel.add(new JLabel("remote port"));
        connectPanel.add(remotePortTF);
        connectPanel.add(new JLabel("remote host"));
        connectPanel.add(remoteAddressTF);
        connectPanel.add(connectSocketButton); 
        connectPanel.add(connectSerialButton);
        connectPanel.add(uploadPacketsButton);
        
        
        sendPacketsButton = new JButton("SEND PACKETS");
        setColorFalse(sendPacketsButton);
        sendPacketsButton.addActionListener(conn_lis);
        sendResetButton = new JButton("SEND GLOBAL RESET");
        setColorFalse(sendResetButton);
        sendResetButton.addActionListener(conn_lis);
        packetPreviewBytesButton = new JButton("SHOW BYTES");
        packetPreviewBytesButton.setBackground(colorFalse);
        packetPreviewBytesButton.addActionListener(conn_lis);
        packetPreviewBytesButton.setVisible(false);
        
        sendPanel.add(sendPacketsButton);
        sendPanel.add(sendResetButton);
        sendPanel.add(packetPreviewBytesButton);
       

        clearAllButton = new JButton("CLEAR ALL PACKETS");
        clearAllButton.addActionListener(packet_lis);
        packetSourceButton = new JButton("PACKETS: USER");
        packetSourceButton.addActionListener(packet_lis);

        playfilePicker = new JFileChooser(System.getProperty("user.dir"));
        playfilePicker.setAcceptAllFileFilterUsed(false);
        playfilePicker.setFileFilter(new FileNameExtensionFilter("Playfiles (*.play, *.xls, *.csv)", "play", "xls", "csv"));
        playfilePathLabel = new JLabel("<no playfile chosen>");
        pickPlayfileButton = new JButton("CHOOSE PLAYFILE");
        pickPlayfileButton.addActionListener(file_lis);

        fileIOPanel.add(pickPlayfileButton);
        fileIOPanel.add(playfilePathLabel);
        
        packetControlPanel.add(clearAllButton);
        packetControlPanel.add(packetSourceButton);


        packetPreviewButton = new JButton("PREVIEW PACKETS");
        packetPreviewButton.setBackground(colorFalse);
        packetPreviewButton.addActionListener(preview_lis);
        packetPreviewNextButton = new JButton("NEXT");
        packetPreviewNextButton.setBackground(colorTrue);
        packetPreviewNextButton.addActionListener(preview_lis);
        packetPreviewNextButton.setVisible(false);
        packetPreviewDoneButton = new JButton("DONE");
        packetPreviewDoneButton.setBackground(colorTrue);
        packetPreviewDoneButton.addActionListener(preview_lis);
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
        
        bottomControlPanel.add(connectPanel);
        bottomControlPanel.add(sendPanel);
        bottomControlPanel.add(statusTextPanel);
        bottomControlPanel.add(previewButtonPanel);


        JPanel topPacketControlPanel = new JPanel();
        BoxLayout topPacketControlBox = new BoxLayout(topPacketControlPanel, BoxLayout.X_AXIS);
        
        addPacketButton = new JButton("NEW PACKET");
        addPacketButton.addActionListener(packet_lis);
        addPacketButton.setForeground(Color.GREEN);
        removePacketButton = new JButton("REMOVE PACKET");
        addPacketButton.setForeground(Color.RED);
        removePacketButton.addActionListener(packet_lis);

        pnumLabel = new JLabel("Current Packet: 0");

        topPacketControlPanel.add(pnumLabel);
        topPacketControlPanel.add(addPacketButton);
        topPacketControlPanel.add(removePacketButton);
       

        JPanel topControlPanel = new JPanel();
        BoxLayout topControlBox = new BoxLayout(topControlPanel, BoxLayout.Y_AXIS);
      
        topControlPanel.add(topPacketControlPanel);
        topControlPanel.add(packetControlPanel);
        topControlPanel.add(fileIOPanel);
        
         
        guiframe.getContentPane().setLayout(new BoxLayout(guiframe.getContentPane(), BoxLayout.Y_AXIS));
        guiframe.getContentPane().add(topControlPanel);
        guiframe.getContentPane().add(channelsPanel);
        guiframe.getContentPane().add(bottomControlPanel);
        guiframe.setSize(800,500);
        guiframe.setVisible(true);

        setChannelButtonsEnabled(false);
        setSocketSendButtonsEnabled(false);
        setSerialSendButtonsEnabled(false);
    }

    void clearPacketDisplay() {
        pnumLabel.setText("Current Packet: 0");
        //default settings
        for (int i=0; i<utils.NCHANNELS; i++) {
            setColorFalse(chanOptButtons[i][INC_OPT]);
            setColorFalse(chanOptButtons[i][POL_OPT]);
            setColorFalse(chanOptButtons[i][DIR_OPT]);
            setColorFalse(chanOptButtons[i][CUR_OPT]);
            chanCurrentTFs[i].setText("0.0");
        }
        if (file_packets.isEmpty() && user_packets.isEmpty()) 
            setChannelButtonsEnabled(false);
    }
    
    void setChannelButtonsEnabled(boolean en) {
        for (int i=0; i<utils.NCHANNELS; i++) {
            chanOptButtons[i][INC_OPT].setEnabled(en);
            chanOptButtons[i][POL_OPT].setEnabled(en);
            chanOptButtons[i][DIR_OPT].setEnabled(en);
            chanOptButtons[i][CUR_OPT].setEnabled(en);
            chanCurrentTFs[i].setEnabled(en);
        }
        removePacketButton.setEnabled(en);
        packetPreviewButton.setEnabled(en);
        clearAllButton.setEnabled(en);
    }

    void setSocketSendButtonsEnabled(boolean en) {
        uploadPacketsButton.setEnabled(en);
        if (!en) setSerialSendButtonsEnabled(en);
    }

    void setSerialSendButtonsEnabled(boolean en) {
        sendPacketsButton.setEnabled(en);
        sendResetButton.setEnabled(en);
    }

    boolean isColorTrue(JComponent comp) {
        return comp.getBackground().equals(colorTrue);
    }
    void setColorTrue(JComponent comp) {
        comp.setBackground(colorTrue);
    }
    void setColorFalse(JComponent comp) {
        comp.setBackground(colorFalse);
    }

    void triggerEventIfColorFalse(JComponent comp, String cmdStr) {
        if (!isColorTrue(comp)) {
            for (ActionListener a : comp.getListeners(ActionListener.class)) {
                a.actionPerformed(new ActionEvent(comp, ActionEvent.ACTION_PERFORMED, cmdStr) {});
            }
        }
    }
    
    String getCurrentString(double current) {
        return decformatter.format(new Double(current));
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
    
    void previewPacketBytes() {
        packetPreviewBytesButton.setBackground(colorTrue);
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

    void reloadPacketDisplay() {
        if (sourceIsFile && !file_packets.isEmpty()) 
            reloadPacketDisplay(file_packets.get(file_pnum), file_pnum);
        else if (!user_packets.isEmpty()) 
            reloadPacketDisplay(user_packets.get(user_pnum), user_pnum);
        else 
            clearPacketDisplay();
    }
    void reloadPacketDisplay(catheterPacket p, int pnum) {
        pnumLabel.setText("Current Packet: "+pnum);
        if (!chanOptButtons[0][0].isEnabled()) 
            setChannelButtonsEnabled(true);
        for (int i=0; i<utils.NCHANNELS; i++) {
            if (p.isChannelIncluded(i)) {
                if (p.getChannelPoll(i))
                    chanOptButtons[i][0].setBackground(colorTrue);
                else
                    chanOptButtons[i][0].setBackground(colorFalse);

                if (p.getChannelDirection(i).equals("positive"))
                    chanOptButtons[i][0].setText("DIR_POS");
                else
                    chanOptButtons[i][0].setText("DIR_NEG");

                chanCurrentTFs[i].setText(getCurrentString(p.getChannelCurrentMA(i)));
            }
        }
    }



    public static void main(String[] args){
        cjg gui = new cjg();
    }

    
    class ChanIncListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            if (sourceIsFile && file_packets.isEmpty()) return;
            if (!sourceIsFile && user_packets.isEmpty()) return;

            for (int i=0; i<utils.NCHANNELS; i++) {
                if (e.getSource().equals(chanOptButtons[i][INC_OPT])) {
                    if (sourceIsFile)
                        toggleInclude(i, file_packets, file_pnum);
                    else 
                        toggleInclude(i, user_packets, user_pnum);
                    break;
                }
            }
        }
        void toggleInclude(int channel, ArrayList<catheterPacket> packets, int pnum) {
            if (!isColorTrue(chanOptButtons[channel][INC_OPT])) {
                packets.get(pnum).includeChannel(channel);
                setColorTrue(chanOptButtons[channel][INC_OPT]);
            } else {
                packets.get(pnum).disableChannel(channel);
                setColorFalse(chanOptButtons[channel][INC_OPT]);
            }
        }
    }

    class ChanPollListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            if (sourceIsFile && file_packets.isEmpty()) return;
            if (!sourceIsFile && user_packets.isEmpty()) return;

            for (int i=0; i<utils.NCHANNELS; i++) {
                if (e.getSource().equals(chanOptButtons[i][POL_OPT])) {
                    if (sourceIsFile)
                        togglePoll(i, file_packets, file_pnum);
                    else
                        togglePoll(i, user_packets, user_pnum);
                    break;
                }
            }
        }
        void togglePoll(int channel, ArrayList<catheterPacket> packets, int pnum) {
            if (!isColorTrue(chanOptButtons[channel][POL_OPT])) {
                triggerEventIfColorFalse(chanOptButtons[channel][INC_OPT], chanOptButtons[channel][INC_OPT].getText());
                packets.get(pnum).setChannelPoll(channel, true);
                setColorTrue(chanOptButtons[channel][POL_OPT]);
            } else {
                packets.get(pnum).setChannelPoll(channel, false);
                setColorFalse(chanOptButtons[channel][POL_OPT]);
            }
        }
    }

    class ChanDirListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            if (sourceIsFile && file_packets.isEmpty()) return;
            if (!sourceIsFile && user_packets.isEmpty()) return;

            for (int i=0; i<utils.NCHANNELS; i++) {
                if (e.getSource().equals(chanOptButtons[i][DIR_OPT])) {
                    if (sourceIsFile)
                        toggleDirection(i, file_packets, file_pnum);
                    else
                        toggleDirection(i, user_packets, user_pnum);
                    break;
                }
            }
        }
        void toggleDirection(int channel, ArrayList<catheterPacket> packets, int pnum) {
            if (chanOptButtons[channel][DIR_OPT].getText().equals("DIR_POS")) {
                triggerEventIfColorFalse(chanOptButtons[channel][INC_OPT], chanOptButtons[channel][INC_OPT].getText());
                packets.get(pnum).setChannelDirection(channel, "negative");
                chanOptButtons[channel][DIR_OPT].setText("DIR_NEG");
            } else {
                packets.get(pnum).setChannelDirection(channel, "positive");
                chanOptButtons[channel][DIR_OPT].setText("DIR_POS");
            }
        }
    }


    class ChanCurrentListener implements ActionListener { //the apply current button
        public void actionPerformed(ActionEvent e) {
            if (sourceIsFile && file_packets.isEmpty()) return;
            if (!sourceIsFile && user_packets.isEmpty()) return;

            for (int i=0; i<utils.NCHANNELS; i++) {
                if (e.getSource().equals(chanOptButtons[i][CUR_OPT])) {
                    if (sourceIsFile)
                        toggleApplyCurrent(i, file_packets, file_pnum);
                    else
                        toggleApplyCurrent(i, user_packets, user_pnum);
                    break;
                } else if (e.getSource().equals(chanCurrentTFs[i])) {
                    updateChannelCurrent(i);
                }
            }
        }
        void toggleApplyCurrent(int channel, ArrayList<catheterPacket> packets, int pnum) {
            if (!isColorTrue(chanOptButtons[channel][CUR_OPT])) {
                triggerEventIfColorFalse(chanOptButtons[channel][INC_OPT], chanOptButtons[channel][INC_OPT].getText());
                updateChannelCurrent(channel); 
                setColorTrue(chanOptButtons[channel][CUR_OPT]);
            } else { //un-apply current
                double newcurrent = packets.get(pnum).resetChannelCurrentMA(channel);            
                chanCurrentTFs[channel].setText(getCurrentString(newcurrent));
                setColorFalse(chanOptButtons[channel][CUR_OPT]);
            }
        }
        void updateChannelCurrent(int channel) {
            if (sourceIsFile)
                updateChannelCurrent(channel, file_packets, file_pnum);
            else 
                updateChannelCurrent(channel, user_packets, user_pnum);
        }
        void updateChannelCurrent(int channel, ArrayList<catheterPacket> packets, int pnum) {
            String current;
            double newcurrent;
            try {
                current = chanCurrentTFs[channel].getText(); 
                if (current == null || current.equals("")) {
                    newcurrent = packets.get(pnum).resetChannelCurrentMA(channel);            
                    chanCurrentTFs[channel].setText(getCurrentString(newcurrent));
                } else {
                    try {
                        newcurrent = Double.valueOf(current);
                        if (newcurrent < 0) throw new NumberFormatException();
                        packets.get(pnum).setChannelCurrentMA(channel, newcurrent);
                    } catch (NumberFormatException nfe) {
                        newcurrent = packets.get(pnum).resetChannelCurrentMA(channel);            
                        chanCurrentTFs[channel].setText(getCurrentString(newcurrent));
                    }
                }
            } catch (NullPointerException e) {
                newcurrent = packets.get(pnum).resetChannelCurrentMA(channel);            
                chanCurrentTFs[channel].setText(getCurrentString(newcurrent));
            }
        }
    }


    class PreviewListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            if (sourceIsFile && file_packets.isEmpty()) return;
            if (!sourceIsFile && user_packets.isEmpty()) return;

            if (e.getSource().equals(packetPreviewButton)) {
                beginPreview();
            } else if (packetPreviewNextButton.isVisible() && e.getSource().equals(packetPreviewNextButton)) {
                previewNextPacket();
            } else if (packetPreviewDoneButton.isVisible() && e.getSource().equals(packetPreviewDoneButton)) {
                endPreview();
            }
        }
        void beginPreview() {
            if (sourceIsFile) {
                if (file_packets.size() > 1)
                    setColorTrue(packetPreviewNextButton);
            } else {
                if (user_packets.size() > 1)
                    setColorTrue(packetPreviewNextButton);
            } 
            packetPreviewNextButton.setVisible(true);
            packetPreviewDoneButton.setVisible(true);
            packetPreviewNum = 0;
            pnumLabel.setText("Current Packet: 0");
            previewNextPacket();
        }

        void previewNextPacket() {
            if (sourceIsFile)
                previewNextPacket(file_packets);
            else 
                previewNextPacket(user_packets);
        }
        void previewNextPacket(ArrayList<catheterPacket> packets) {
            pnumLabel.setText("Current Packet: "+packetPreviewNum); 
            clearPacketPreviewText();
            statusText.append(packets.get(packetPreviewNum).present()+"\n");
            packetPreviewNum = (packetPreviewNum + 1) % packets.size();
        }
        void endPreview() {
            packetPreviewNextButton.setVisible(false);
            packetPreviewDoneButton.setVisible(false);
            clearPacketPreviewText();
            reloadPacketDisplay();
        }
    }


    class PacketsListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            if (e.getSource().equals(addPacketButton))
                addNewPacket();
            else if (e.getSource().equals(removePacketButton))
                removePacket();
            else if (e.getSource().equals(packetSourceButton))
                togglePacketSource();
        }
        void addNewPacket() {
            if (sourceIsFile) {
                if (file_packets.isEmpty()) return;
                file_packets.add(new catheterPacket(utils.NCHANNELS));
                //file_pnum++;
                if (!chanOptButtons[0][0].isEnabled()) 
                    setChannelButtonsEnabled(true);
            } else {
                if (user_packets.isEmpty())
                    setChannelButtonsEnabled(true);
                //user_pnum++;
                user_packets.add(new catheterPacket(utils.NCHANNELS));
            }
            reloadPacketDisplay();
        }
        void removePacket() {
            if (sourceIsFile) removePacket(file_packets, file_pnum);
            else removePacket(user_packets, user_pnum);
        } 
        void removePacket(ArrayList<catheterPacket> packets, int pnum) {
            if (packets.isEmpty()) return;
            packets.remove(pnum);
            if (pnum > 0) pnum -= 1;
            pnumLabel.setText("Current Packet: "+pnum);
            reloadPacketDisplay();
        }
        void togglePacketSource () {
            sourceIsFile = !sourceIsFile;
            if (sourceIsFile) {
                packetSourceButton.setText("PACKETS: FILE");
                if (playfile == null) return;
                catheterPacket.loadPlayfile(playfile, file_packets);
            } else {
                packetSourceButton.setText("PACKETS: USER");
                if (user_packets.isEmpty()) return;
            }
            setPacketNum(0);
            reloadPacketDisplay();
        }
        void setPacketNum(int pnum) {
            if (sourceIsFile) file_pnum = pnum;
            else user_pnum = pnum;
            pnumLabel.setText("Current Packet: "+pnum);
        }
    }


    class FilesListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            if (e.getSource().equals(pickPlayfileButton)) {
                int ret = playfilePicker.showOpenDialog(guiframe); 
                if (ret == JFileChooser.APPROVE_OPTION) { 
                    playfile = playfilePicker.getSelectedFile();
                    playfilePathLabel.setText(playfile.getName());
                    if (sourceIsFile) {
                        file_packets = catheterPacket.loadPlayfile(playfile, file_packets);
                        if (!file_packets.isEmpty())
                            reloadPacketDisplay();
                        else playfilePathLabel.setText("failed to load playfile.");
                    }
                }
            }
        }
    }


    class ConnectionListener implements ActionListener {
        public void actionPerformed(ActionEvent e) {
            if (e.getSource().equals(connectSocketButton))
                remoteConnect();
            else if (e.getSource().equals(connectSerialButton))
                serialConnect();
            else if (e.getSource().equals(uploadPacketsButton))
                uploadPackets();
            else if (e.getSource().equals(sendPacketsButton))
                sendPackets();
            else if (e.getSource().equals(sendResetButton))
                sendGlobalReset();
            else if (e.getSource().equals(packetPreviewBytesButton))
                previewPacketBytes();
        }
        void remoteConnect() {
            sock.setRemotePort(Integer.parseInt(remotePortTF.getText()));
            sock.setRemoteAddress(remoteAddressTF.getText());
            if (connectSocketButton.getBackground().equals(colorFalse)) {
                if (!sock.connect()) {
                    changeStatusLine("Could not connect to remote server.\n", 0);
                    setSocketSendButtonsEnabled(false);
                } else {
                    connectSocketButton.setBackground(colorTrue);
                    connectSocketButton.setText("DISCONNECT REMOTE");
                    changeStatusLine("Connected to remote.\n", 0);
                    setSocketSendButtonsEnabled(true);
                }
            } else {
                if (!sock.close())
                    changeStatusLine("Failed to close remote server.\n", 0);
                else {
                    connectSocketButton.setBackground(colorFalse);
                    connectSocketButton.setText("CONNECT REMOTE");
                    changeStatusLine("Disconnected from remote.\n", 0);
                    setSocketSendButtonsEnabled(false);
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
                setSerialSendButtonsEnabled(false);
            } else {
                connectSerialButton.setBackground(colorTrue);
                connectSerialButton.setText("DISCONNECT SERIAL");
                changeStatusLine("Connected to serial controller.\n", 1);
                setSerialSendButtonsEnabled(true);
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
                sock.sendPacket1(utils.PACKET_NUM, i);
                for (int j=0; j<utils.NCHANNELS; j++) {
                    if (packets.get(i).isChannelIncluded(j)) {
                        sock.sendPacket1(utils.CHANNEL_NUM, j);
                        sock.sendPacket1(utils.POLL, packets.get(i).getChannelPoll(j));
                        sock.sendPacket1(utils.CURRENT, packets.get(i).getChannelCurrentRes(j));
                        sock.sendPacket1(utils.DIRECTION, packets.get(i).getChannelDirection(j));
                        sock.sendPacket1(utils.DELAY, packets.get(i).getDelayMS());
                    }
                }
            }
            packetPreviewBytesButton.setBackground(colorFalse);
            packetPreviewBytesButton.setVisible(true);
            return true; 
        }
        void sendPackets() {
            sock.sendPacket1(utils.SEND_PACKETS, 0);
            packetPreviewBytesButton.setBackground(colorFalse);
            packetPreviewBytesButton.setVisible(false);
        }
        void sendGlobalReset() {
            sock.sendPacket1(utils.SEND_GLOBAL_RESET, 0);
        }
        void previewPacketBytes() {
            sock.sendPacket1(utils.PREVIEW_PACKETS, 0);
        }
    }
} 
