/* gui_main.cpp */

#include "common_utils.h"
#include "pc_utils.h"
#include "wx_utils.h"

#include <cstring>
#include <vector>
#include <cstdio>


/* global variables */
std::vector<struct strictCatheterPacket> packetVectFile;
std::vector<struct strictCatheterPacket> packetVectWindow;
unsigned int currentWindowPacket;
struct files_t files;
bool connected;
bool playfilePathSelected;
bool cmdWindowEmpty;
bool sourceIsPlayfile;
bool logging;

/* begin gui utils methods */
bool gui_serial_connect() {
    return true;
}

bool gui_serial_disconnect() {
    return true;
}

void gui_cleanup() {
    //close any open files
    //close port connection if connected
    gui_serial_disconnect();
}

void gui_startup() {
    connected = false;
    playfilePathSelected = false;
    cmdWindowEmpty = true;
    sourceIsPlayfile=true;
    logging=false;
    currentWindowPacket=1;
    struct strictCatheterPacket p;
    packetVectWindow.push_back(p);
}
/* end gui utils methods */

/* begin wxWidgets methods */
bool CatheterApp::OnInit() {
    CatheterFrame *frame = new CatheterFrame("Catheter Gui", wxPoint(50,50), wxSize(600,800));
    frame->Show(true);
    return true;
}

CatheterFrame::CatheterFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {

    int fw, fh;
    this->GetClientSize(&fw, &fh);
    /*wxRect frameRect = GetScreenRect();
    fw = frameRect.GetWidth();
    fh = frameRect.GetHeight();*/

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT);

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar(); //line of text at bottom of window
    SetStatusText("Welcome to the Catheter Control Gui. Controller is currently DISCONNECTED.");

    controlPanel = new wxPanel(this, wxID_ANY);

    wxSizer *controlSizerV = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer *fileSizerH = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *packetSizerV = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *serialSizerH = new wxBoxSizer(wxHORIZONTAL);
   
    wxBoxSizer *loadPlayfileSizerV = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *savePlayfileSizerV = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *logfileSizerV = new wxBoxSizer(wxVERTICAL);
    
    loadPlayfileSizerV->Add(new wxButton(controlPanel, ID_LOAD_PLAYFILE_BUTTON, wxT("Load Playfile")));
    loadPlayfileSizerV->Add(new wxTextCtrl(controlPanel, ID_LOAD_PLAYFILE_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));
    
    savePlayfileSizerV->Add(new wxButton(controlPanel, ID_SAVE_PLAYFILE_BUTTON, wxT("Save to Playfile")));
    savePlayfileSizerV->Add(new wxTextCtrl(controlPanel, ID_SAVE_PLAYFILE_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));
    
    logfileSizerV->Add(new wxButton(controlPanel, ID_LOGFILE_BUTTON, wxT("Log to File")));
    logfileSizerV->Add(new wxTextCtrl(controlPanel, ID_LOGFILE_TEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));

    fileSizerH->Add(loadPlayfileSizerV);
    fileSizerH->AddSpacer(5);
    fileSizerH->Add(savePlayfileSizerV);
    fileSizerH->AddSpacer(5);
    fileSizerH->Add(logfileSizerV);
    
    //int ids[7] = {ID_INC_CHAN1, ID_CURRENT_TEXT1, ID_CURRENT_BUTTON1, ID_POLL_CHAN1, ID_ENA_CHAN1, ID_UPD_CHAN1, ID_DIR_CHAN1};
    //wxBoxSizesr *chan1SizerH = CatheterFrame::CreateChannelRowSizer(ids);
    wxBoxSizer *chan1SizerH = new wxBoxSizer(wxHORIZONTAL);
    chan1SizerH->Add(new wxCheckBox(controlPanel, ID_INC_CHAN1, wxEmptyString));
    chan1SizerH->Add(new wxTextCtrl(controlPanel, ID_CURRENT_TEXT1, wxString("0.0"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));
    chan1SizerH->Add(new wxButton(controlPanel, ID_CURRENT_BUTTON1, wxString("Apply")));
    chan1SizerH->Add(new wxCheckBox(controlPanel, ID_POLL_CHAN1, wxEmptyString));
    chan1SizerH->Add(new wxCheckBox(controlPanel, ID_ENA_CHAN1, wxEmptyString));
    chan1SizerH->Add(new wxCheckBox(controlPanel, ID_UPD_CHAN1, wxEmptyString));
    chan1SizerH->Add(new wxCheckBox(controlPanel, ID_DIR_CHAN1, wxEmptyString));

    
    wxBoxSizer *incChanSizerV = new wxBoxSizer(wxVERTICAL);
    incChanSizerV->Add(new wxStaticText(controlPanel, wxID_ANY, wxString("include")));
    incChanSizerV->Add(new wxCheckBox(controlPanel, ID_INC_CHAN2, wxEmptyString));
    incChanSizerV->Add(new wxCheckBox(controlPanel, ID_INC_CHAN3, wxEmptyString));
    incChanSizerV->Add(new wxCheckBox(controlPanel, ID_INC_CHAN4, wxEmptyString));
    incChanSizerV->Add(new wxCheckBox(controlPanel, ID_INC_CHAN5, wxEmptyString));
    incChanSizerV->Add(new wxCheckBox(controlPanel, ID_INC_CHAN6, wxEmptyString));

    wxBoxSizer *currentSizerV = new wxBoxSizer(wxVERTICAL);
    currentSizerV->Add(new wxStaticText(controlPanel, wxID_ANY, wxString("current (mA)")));
    currentSizerV->Add(new wxTextCtrl(controlPanel, ID_CURRENT_TEXT2, wxString("0.0"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));
    currentSizerV->Add(new wxTextCtrl(controlPanel, ID_CURRENT_TEXT3, wxString("0.0"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));
    currentSizerV->Add(new wxTextCtrl(controlPanel, ID_CURRENT_TEXT4, wxString("0.0"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));
    currentSizerV->Add(new wxTextCtrl(controlPanel, ID_CURRENT_TEXT5, wxString("0.0"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));
    currentSizerV->Add(new wxTextCtrl(controlPanel, ID_CURRENT_TEXT6, wxString("0.0"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));

    wxBoxSizer *currentButtonSizerV = new wxBoxSizer(wxVERTICAL);
    currentButtonSizerV->Add(new wxStaticText(controlPanel, wxID_ANY, wxEmptyString));
    currentButtonSizerV->Add(new wxButton(controlPanel, ID_CURRENT_BUTTON2, wxString("Apply")));
    currentButtonSizerV->Add(new wxButton(controlPanel, ID_CURRENT_BUTTON3, wxString("Apply")));
    currentButtonSizerV->Add(new wxButton(controlPanel, ID_CURRENT_BUTTON4, wxString("Apply")));
    currentButtonSizerV->Add(new wxButton(controlPanel, ID_CURRENT_BUTTON5, wxString("Apply")));
    currentButtonSizerV->Add(new wxButton(controlPanel, ID_CURRENT_BUTTON6, wxString("Apply")));

    wxBoxSizer *pollSizerV = new wxBoxSizer(wxVERTICAL);
    pollSizerV->Add(new wxStaticText(controlPanel, wxID_ANY, wxString("poll")));
    pollSizerV->Add(new wxCheckBox(controlPanel, ID_POLL_CHAN2, wxEmptyString));
    pollSizerV->Add(new wxCheckBox(controlPanel, ID_POLL_CHAN3, wxEmptyString));
    pollSizerV->Add(new wxCheckBox(controlPanel, ID_POLL_CHAN4, wxEmptyString));
    pollSizerV->Add(new wxCheckBox(controlPanel, ID_POLL_CHAN5, wxEmptyString));
    pollSizerV->Add(new wxCheckBox(controlPanel, ID_POLL_CHAN6, wxEmptyString));

    wxBoxSizer *enaSizerV = new wxBoxSizer(wxVERTICAL);
    enaSizerV->Add(new wxStaticText(controlPanel, wxID_ANY, wxString("enable")));
    enaSizerV->Add(new wxCheckBox(controlPanel, ID_ENA_CHAN2, wxEmptyString));
    enaSizerV->Add(new wxCheckBox(controlPanel, ID_ENA_CHAN3, wxEmptyString));
    enaSizerV->Add(new wxCheckBox(controlPanel, ID_ENA_CHAN4, wxEmptyString));
    enaSizerV->Add(new wxCheckBox(controlPanel, ID_ENA_CHAN5, wxEmptyString));
    enaSizerV->Add(new wxCheckBox(controlPanel, ID_ENA_CHAN6, wxEmptyString));

    wxBoxSizer *updSizerV = new wxBoxSizer(wxVERTICAL);
    updSizerV->Add(new wxStaticText(controlPanel, wxID_ANY, wxString("update")));
    updSizerV->Add(new wxCheckBox(controlPanel, ID_UPD_CHAN2, wxEmptyString));
    updSizerV->Add(new wxCheckBox(controlPanel, ID_UPD_CHAN3, wxEmptyString));
    updSizerV->Add(new wxCheckBox(controlPanel, ID_UPD_CHAN4, wxEmptyString));
    updSizerV->Add(new wxCheckBox(controlPanel, ID_UPD_CHAN5, wxEmptyString));
    updSizerV->Add(new wxCheckBox(controlPanel, ID_UPD_CHAN6, wxEmptyString));

    wxBoxSizer *dirSizerV = new wxBoxSizer(wxVERTICAL);
    dirSizerV->Add(new wxStaticText(controlPanel, wxID_ANY, wxString("direction")));
    dirSizerV->Add(new wxCheckBox(controlPanel, ID_DIR_CHAN2, wxEmptyString));
    dirSizerV->Add(new wxCheckBox(controlPanel, ID_DIR_CHAN3, wxEmptyString));
    dirSizerV->Add(new wxCheckBox(controlPanel, ID_DIR_CHAN4, wxEmptyString));
    dirSizerV->Add(new wxCheckBox(controlPanel, ID_DIR_CHAN5, wxEmptyString));
    dirSizerV->Add(new wxCheckBox(controlPanel, ID_DIR_CHAN6, wxEmptyString));

    wxBoxSizer *singlePacketSizerH = new wxBoxSizer(wxHORIZONTAL);

    singlePacketSizerH->Add(incChanSizerV);
    singlePacketSizerH->AddSpacer(15);
    singlePacketSizerH->Add(currentSizerV);
    singlePacketSizerH->Add(currentButtonSizerV);
    singlePacketSizerH->AddSpacer(15);
    singlePacketSizerH->Add(pollSizerV);
    singlePacketSizerH->AddSpacer(15);
    singlePacketSizerH->Add(enaSizerV);
    singlePacketSizerH->AddSpacer(15);
    singlePacketSizerH->Add(updSizerV);
    singlePacketSizerH->AddSpacer(15);
    singlePacketSizerH->Add(dirSizerV);

    wxBoxSizer *packetButtonSizerH = new wxBoxSizer(wxHORIZONTAL);
    packetButtonSizerH->Add(new wxButton(controlPanel, ID_CLEARPACKET_BUTTON, wxT("Clear Packet")));
    packetButtonSizerH->AddSpacer(5);
    packetButtonSizerH->Add(new wxButton(controlPanel, ID_CLEARALLPACKETS_BUTTON, wxT("Clear All Packets")));
    packetButtonSizerH->AddSpacer(15);
    packetButtonSizerH->Add(new wxButton(controlPanel, ID_PREVPACKET_BUTTON, wxT("<- Prev")), 0, wxALIGN_CENTRE_HORIZONTAL, 5);
    packetButtonSizerH->AddSpacer(5);
    packetButtonSizerH->Add(new wxButton(controlPanel, ID_ADDPACKET_BUTTON, wxT("++ Add")));
    packetButtonSizerH->AddSpacer(5);
    packetButtonSizerH->Add(new wxButton(controlPanel, ID_NEXTPACKET_BUTTON, wxT("Next ->")));
    packetButtonSizerH->AddSpacer(15);
    packetButtonSizerH->Add(new wxButton(controlPanel, ID_PREVIEWPACKETS_BUTTON, wxT("Preview Packets")), 0, wxALIGN_RIGHT, 5);

    packetSizerV->Add(singlePacketSizerH);
    packetSizerV->AddSpacer(5);
    packetSizerV->Add(packetButtonSizerH);

    serialSizerH->Add(new wxButton(controlPanel, ID_CONNECT_BUTTON, wxT("Connect")));
    serialSizerH->AddSpacer(5);
    serialSizerH->Add(new wxButton(controlPanel, ID_SENDPACKETS_BUTTON, wxT("Send Packets")));
    serialSizerH->AddSpacer(5);
    serialSizerH->Add(new wxButton(controlPanel, ID_GLOBALRESET_BUTTON, wxT("Send Global Reset")));
    serialSizerH->AddSpacer(5);
    serialSizerH->Add(new wxRadioButton(controlPanel, ID_SOURCEPLAYFILE, wxT("Read Packets\nfrom Playfile"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP));
    serialSizerH->AddSpacer(5);
    serialSizerH->Add(new wxRadioButton(controlPanel, ID_SOURCEWINDOW, wxT("Read Packets\nfrom Window")));

    controlSizerV->Add(fileSizerH, wxSizerFlags().Left().Expand().Border(wxALL, 10));
    controlSizerV->Add(packetSizerV, wxSizerFlags().Left().Expand().Border(wxALL, 10));
    controlSizerV->Add(serialSizerH, wxSizerFlags().Left().Expand().Border(wxALL, 10));
    
    controlPanel->SetSizer(controlSizerV);

    gui_startup();
}

wxBoxSizer *CatheterFrame::CreateChannelRowSizer(int ids[]) {

    wxBoxSizer *chanSizerH = new wxBoxSizer(wxHORIZONTAL);
    chanSizerH->Add(new wxCheckBox(controlPanel, ids[0], wxEmptyString));
    chanSizerH->Add(new wxTextCtrl(controlPanel, ids[1], wxString("0.0"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER));
    chanSizerH->Add(new wxButton(controlPanel, ids[2], wxString("Apply")));
    chanSizerH->Add(new wxCheckBox(controlPanel, ids[3], wxEmptyString));
    chanSizerH->Add(new wxCheckBox(controlPanel, ids[4], wxEmptyString));
    chanSizerH->Add(new wxCheckBox(controlPanel, ids[5], wxEmptyString));
    chanSizerH->Add(new wxCheckBox(controlPanel, ids[6], wxEmptyString));

    return chanSizerH;
}

void CatheterFrame::OnApplyCurrent(int btnId) {
    wxTextCtrl *currentField;
    int channel;
    switch (btnId) {
        case ID_CURRENT_BUTTON1:
            currentField = (wxTextCtrl*)(controlPanel->FindWindow(ID_CURRENT_TEXT1));
            channel=0;
            break;
        case ID_CURRENT_BUTTON2:
            currentField = (wxTextCtrl*)(controlPanel->FindWindow(ID_CURRENT_TEXT2));
            channel=1;
            break;
        case ID_CURRENT_BUTTON3:
            currentField = (wxTextCtrl*)(controlPanel->FindWindow(ID_CURRENT_TEXT3));
            channel=2;
            break;
        case ID_CURRENT_BUTTON4:
            currentField = (wxTextCtrl*)(controlPanel->FindWindow(ID_CURRENT_TEXT4));
            channel=3;
            break;
        case ID_CURRENT_BUTTON5:
            currentField = (wxTextCtrl*)(controlPanel->FindWindow(ID_CURRENT_TEXT5));
            channel=4;
            break;
        case ID_CURRENT_BUTTON6:
            currentField = (wxTextCtrl*)(controlPanel->FindWindow(ID_CURRENT_TEXT6));
            channel=5;
            break;
    }
    double current = atof(static_cast<const char *>((currentField->GetLineText(0)).c_str()));
    unsigned int res;
    dir_t dir;
    
    convertCurrent(current, &res, &dir);
    packetVectWindow[currentWindowPacket].cmds[channel].data12 = res;
    packetVectWindow[currentWindowPacket].cmds[channel].dir = dir;

    wxString msg = "changed channel currnet to ";
    msg += currentField->GetLineText(0);
    wxMessageBox(msg);
}

void CatheterFrame::AddPath(int btnId) {
    //wxMessageBox("AddPath()");
    wxDirDialog dlg(NULL, "Choose file", wxEmptyString, wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    int status = dlg.ShowModal();
    if (status == wxID_OK) {
        wxString pathStr = dlg.GetPath();
        if (!pathStr.IsEmpty()) { 
            const char * filepath = static_cast<const char*>(pathStr.c_str());
            switch (btnId) {
                case ID_LOAD_PLAYFILE_BUTTON:
                    files.paths[fp_playpath] = filepath;
                    playfilePathSelected = true;
                    break;
                case ID_SAVE_PLAYFILE_BUTTON:
                    files.paths[fp_savepath] = filepath;
                    break;
                case ID_LOGFILE_BUTTON:
                    files.paths[fp_logpath] = filepath;
                    break;
            }
        }
    }
}

void CatheterFrame::ClearPacketWindow() {
    wxMessageBox("ClearPacketWindow()");
}

void CatheterFrame::ClearAllPacketWindows() {
    wxMessageBox("ClearAllPacketWindows()");
    /*CatheterFrame::ClearPacketWindow();
    packetVectWindow.clear(); 
    currentWindowPacket=1;*/
}

void CatheterFrame::LoadPacketWindow(struct strictCatheterPacket p) {
    wxMessageBox("LoadPacketWindow()");

}

void CatheterFrame::SwitchPacketView(int jump) {
    wxMessageBox("SwitchPacketView()");
    if ((currentWindowPacket + jump) > packetVectWindow.size() || (currentWindowPacket + jump) < 1) {
        wxMessageBox("requested Packet out of range");
        return;
    } else {
        //save current window information to pckVectWindow[currentWindowPacket]
        //update currentWindowPacket
        currentWindowPacket += jump;
        //TODO: update number in corner that tells what currentWindowPacket is
        //clear window
        CatheterFrame::ClearPacketWindow();
        CatheterFrame::LoadPacketWindow(packetVectWindow[currentWindowPacket]);
    }
}

void CatheterFrame::AddPacket() {
    wxMessageBox("AddPacket()");
    struct strictCatheterPacket p;
    if (currentWindowPacket == packetVectWindow.size())
        packetVectWindow.push_back(p);
    else
        packetVectWindow.insert(packetVectWindow.begin() + currentWindowPacket, p);
    CatheterFrame::SwitchPacketView(1); 
}

void CatheterFrame::PreviewPackets() {
    wxMessageBox("PreviewPackets()");

}

void CatheterFrame::SendPackets() {
    wxMessageBox("SendPackets()");

}
            
void CatheterFrame::SendGlobalResetPacket() {
    wxMessageBox("SendGlobalResetPacket()");

}

void CatheterFrame::OnExit(wxCommandEvent& e) {
    gui_cleanup();
    Close(true);
}

void CatheterFrame::OnAbout(wxCommandEvent& e) {
    wxMessageBox("gui program to control the catheter", "About Catheter Gui", wxOK | wxICON_INFORMATION);
}

void CatheterFrame::OnButton(wxCommandEvent& e) {
    //wxMessageBox("CatheterFrame::OnButton");
    switch (e.GetId()) {
        case ID_LOAD_PLAYFILE_BUTTON:
        case ID_SAVE_PLAYFILE_BUTTON:
        case ID_LOGFILE_BUTTON:
            CatheterFrame::AddPath(e.GetId());
            break;
        case ID_CLEARPACKET_BUTTON:
            CatheterFrame::ClearPacketWindow();
            break;
        case ID_CLEARALLPACKETS_BUTTON:
            CatheterFrame::ClearAllPacketWindows();
            break;
        case ID_PREVPACKET_BUTTON:
            CatheterFrame::SwitchPacketView(-1);
            break;
        case ID_NEXTPACKET_BUTTON:
            CatheterFrame::SwitchPacketView(1);
            break;
        case ID_ADDPACKET_BUTTON:
            CatheterFrame::AddPacket();
            break;
        case ID_PREVIEWPACKETS_BUTTON:
            CatheterFrame::PreviewPackets();
            break;
        case ID_CONNECT_BUTTON:
            ((wxButton*)(e.GetEventObject()))->SetLabel(wxString("Connected"));
            gui_serial_connect();
            break;
        case ID_SENDPACKETS_BUTTON:
            CatheterFrame::SendPackets();
            break;
        case ID_GLOBALRESET_BUTTON:
            CatheterFrame::SendGlobalResetPacket();
            break;
        case ID_SOURCEPLAYFILE:
            sourceIsPlayfile=true;
            break;
        case ID_SOURCEWINDOW:
            sourceIsPlayfile=false;
            break;
        case ID_CURRENT_BUTTON1:
        case ID_CURRENT_BUTTON2:
        case ID_CURRENT_BUTTON3:
        case ID_CURRENT_BUTTON4:
        case ID_CURRENT_BUTTON5:
        case ID_CURRENT_BUTTON6:
            CatheterFrame::OnApplyCurrent(e.GetId());
            break;
    }
}

void CatheterFrame::OnText(wxCommandEvent& e) {
//    wxMessageBox("CatheterFrame::OnText");
}

void CatheterFrame::OnTextEnter(wxCommandEvent& e) {
    wxMessageBox("CatheterFrame::OnTextEnter");
}

void CatheterFrame::OnCheckBox(wxCommandEvent& e) {
    wxMessageBox("CatheterFrame::OnCheckBox");
}

void CatheterFrame::OnRadioButton(wxCommandEvent& e) {
    wxMessageBox("CatheterFrame::OnRadioButton");
}

wxBEGIN_EVENT_TABLE(CatheterFrame, wxFrame)
    EVT_MENU(wxID_EXIT, CatheterFrame::OnExit)
    EVT_MENU(wxID_ABOUT, CatheterFrame::OnAbout)
    EVT_TEXT(ID_CURRENT_TEXT1, CatheterFrame::OnText)
    EVT_TEXT(ID_CURRENT_TEXT2, CatheterFrame::OnText)
    EVT_TEXT(ID_CURRENT_TEXT3, CatheterFrame::OnText)
    EVT_TEXT(ID_CURRENT_TEXT4, CatheterFrame::OnText)
    EVT_TEXT(ID_CURRENT_TEXT5, CatheterFrame::OnText)
    EVT_TEXT(ID_CURRENT_TEXT6, CatheterFrame::OnText)
    EVT_TEXT(ID_LOAD_PLAYFILE_TEXT, CatheterFrame::OnText)
    EVT_TEXT(ID_SAVE_PLAYFILE_TEXT, CatheterFrame::OnText)
    EVT_TEXT(ID_LOGFILE_TEXT, CatheterFrame::OnText)
    EVT_TEXT_ENTER(ID_CURRENT_TEXT1, CatheterFrame::OnTextEnter)
    EVT_TEXT_ENTER(ID_CURRENT_TEXT2, CatheterFrame::OnTextEnter)
    EVT_TEXT_ENTER(ID_CURRENT_TEXT3, CatheterFrame::OnTextEnter)
    EVT_TEXT_ENTER(ID_CURRENT_TEXT4, CatheterFrame::OnTextEnter)
    EVT_TEXT_ENTER(ID_CURRENT_TEXT5, CatheterFrame::OnTextEnter)
    EVT_TEXT_ENTER(ID_CURRENT_TEXT6, CatheterFrame::OnTextEnter)
    EVT_TEXT_ENTER(ID_LOAD_PLAYFILE_TEXT, CatheterFrame::OnTextEnter)
    EVT_TEXT_ENTER(ID_SAVE_PLAYFILE_TEXT, CatheterFrame::OnTextEnter)
    EVT_TEXT_ENTER(ID_LOGFILE_TEXT, CatheterFrame::OnTextEnter)
    EVT_CHECKBOX(ID_INC_CHAN1, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_INC_CHAN2, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_INC_CHAN3, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_INC_CHAN4, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_INC_CHAN5, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_INC_CHAN6, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_POLL_CHAN1, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_POLL_CHAN2, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_POLL_CHAN3, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_POLL_CHAN4, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_POLL_CHAN5, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_POLL_CHAN6, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_ENA_CHAN1, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_ENA_CHAN2, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_ENA_CHAN3, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_ENA_CHAN4, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_ENA_CHAN5, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_ENA_CHAN6, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_UPD_CHAN1, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_UPD_CHAN2, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_UPD_CHAN3, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_UPD_CHAN4, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_UPD_CHAN5, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_UPD_CHAN6, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_DIR_CHAN1, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_DIR_CHAN2, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_DIR_CHAN3, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_DIR_CHAN4, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_DIR_CHAN5, CatheterFrame::OnCheckBox)
    EVT_CHECKBOX(ID_DIR_CHAN6, CatheterFrame::OnCheckBox)
    EVT_BUTTON(ID_LOAD_PLAYFILE_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_SAVE_PLAYFILE_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_LOGFILE_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CLEARPACKET_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CLEARALLPACKETS_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_PREVPACKET_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_ADDPACKET_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_NEXTPACKET_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_PREVIEWPACKETS_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CONNECT_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_SENDPACKETS_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_GLOBALRESET_BUTTON, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CURRENT_BUTTON1, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CURRENT_BUTTON2, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CURRENT_BUTTON3, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CURRENT_BUTTON4, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CURRENT_BUTTON5, CatheterFrame::OnButton)
    EVT_BUTTON(ID_CURRENT_BUTTON6, CatheterFrame::OnButton)
    EVT_RADIOBUTTON(ID_SOURCEPLAYFILE, CatheterFrame::OnRadioButton)
    EVT_RADIOBUTTON(ID_SOURCEWINDOW, CatheterFrame::OnRadioButton)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(CatheterApp);
/* end wxWidgets methods */
