///
/// \file
///



#ifndef GDW_PANEL_H
#define GDW_PANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class gdw_panel : public wxPanel
{
 public:
  gdw_panel(wxWindow *parent);
  ~gdw_panel();
  
 private:
  void event_handler  (wxCommandEvent& event);
};

#endif
