#ifndef TOPFRAME_H
#define TOPFRAME_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/notebook.h>
#include "database.h"

#define PROG_VERSION "Genealogical Data Explorer\n\nVersion 0.1"

class TopFrame : public wxFrame
{
public:
  TopFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
  
private:
  void event_handler  (wxCommandEvent& event);
  
  enum
    {
      ID_Connect = 1,
      ID_Disconnect,
      ID_ShowLogWin,
      ID_DeletePage,
      ID_Edit,
      ID_first = ID_Connect,
      ID_last  = ID_Edit
    };


  void OnConnect      (wxCommandEvent& event);
  void OnDisconnect   (wxCommandEvent& event);
  void OnEdit         (wxCommandEvent& event);

  wxPanel     *top_panel;
  wxNotebook  *notebook;
  database    gendat_db;

};

#endif
