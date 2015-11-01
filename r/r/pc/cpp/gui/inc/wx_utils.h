/* wx_utils.h */

#ifndef WX_UTILS_H
#define WX_UTILS_H

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

enum {
    ID_LOAD_PLAYFILE_BUTTON,
    ID_SAVE_PLAYFILE_BUTTON,
    ID_LOGFILE_BUTTON,
    ID_LOAD_PLAYFILE_TEXT,
    ID_SAVE_PLAYFILE_TEXT,
    ID_LOGFILE_TEXT,
    
    ID_CURRENT_TEXT1,
    ID_CURRENT_TEXT2,
    ID_CURRENT_TEXT3,
    ID_CURRENT_TEXT4,
    ID_CURRENT_TEXT5,
    ID_CURRENT_TEXT6,
    
    ID_CURRENT_BUTTON1,
    ID_CURRENT_BUTTON2,
    ID_CURRENT_BUTTON3,
    ID_CURRENT_BUTTON4,
    ID_CURRENT_BUTTON5,
    ID_CURRENT_BUTTON6,
    
    ID_INC_CHAN1,
    ID_INC_CHAN2,
    ID_INC_CHAN3,
    ID_INC_CHAN4,
    ID_INC_CHAN5,
    ID_INC_CHAN6,
    
    ID_POLL_CHAN1,
    ID_POLL_CHAN2,
    ID_POLL_CHAN3,
    ID_POLL_CHAN4,
    ID_POLL_CHAN5,
    ID_POLL_CHAN6,
    
    ID_ENA_CHAN1,
    ID_ENA_CHAN2,
    ID_ENA_CHAN3,
    ID_ENA_CHAN4,
    ID_ENA_CHAN5,
    ID_ENA_CHAN6,
    
    ID_UPD_CHAN1,
    ID_UPD_CHAN2,
    ID_UPD_CHAN3,
    ID_UPD_CHAN4,
    ID_UPD_CHAN5,
    ID_UPD_CHAN6,
    
    ID_DIR_CHAN1,
    ID_DIR_CHAN2,
    ID_DIR_CHAN3,
    ID_DIR_CHAN4,
    ID_DIR_CHAN5,
    ID_DIR_CHAN6,
    
    ID_CLEARPACKET_BUTTON,
    ID_CLEARALLPACKETS_BUTTON,
    ID_PREVPACKET_BUTTON,
    ID_ADDPACKET_BUTTON,
    ID_NEXTPACKET_BUTTON,
    ID_PREVIEWPACKETS_BUTTON,
    ID_CONNECT_BUTTON,
    ID_SENDPACKETS_BUTTON,
    ID_GLOBALRESET_BUTTON,
    ID_SOURCEPLAYFILE,
    ID_SOURCEWINDOW
};

class  CatheterApp: public wxApp {
    public:
        virtual bool OnInit();
};

class CatheterFrame: public wxFrame {
    public:
        CatheterFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

    private:
        wxPanel *controlPanel;

        void OnExit(wxCommandEvent& e);
        void OnAbout(wxCommandEvent& e);
        void OnButton(wxCommandEvent& e);
        void OnText(wxCommandEvent& e);
        void OnTextEnter(wxCommandEvent& e);
        void OnCheckBox(wxCommandEvent& e);
        void OnRadioButton(wxCommandEvent& e);
        void AddPath(int btnId);
        void ClearPacketWindow(void);
        void ClearAllPacketWindows(void);
        void LoadPacketWindow(struct strictCatheterPacket p);
        void SwitchPacketView(int jump);
        void AddPacket(void);
        void PreviewPackets(void);
        void SendPackets(void);
        void SendGlobalResetPacket(void);
        void OnApplyCurrent(int btnId);
        wxBoxSizer * CreateChannelRowSizer(int ids[]);

        wxDECLARE_EVENT_TABLE();
};


#endif //WX_UTILS_H
