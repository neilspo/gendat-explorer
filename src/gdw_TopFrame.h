///
/// \file gdw_TopFrame.h
///


#ifndef TOPFRAME_H
#define TOPFRAME_H

#define PROG_VERSION "Genealogical Data Explorer\n\nVersion 0.1"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/notebook.h>
#include "database.h"
#include "db_map.h"

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
      ID_ReloadPage,
      ID_Execute,
      ID_ShowSourceInfo,
      ID_Edit,
      ID_Search,
      ID_first = ID_Connect,
      ID_last  = ID_Search
    };

  wxPanel            *top_panel;
  wxNotebook         *notebook;
  database            gendat_db;
  db_map  gendat_sources;
};
#endif
