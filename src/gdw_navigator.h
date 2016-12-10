///
/// \file
///



#ifndef GDW_NAVIGATOR_H
#define GDW_NAVIGATOR_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


class gdw_navigator : public gdw_panel
{
 public:
  gdw_navigator(wxWindow *parent);
  ~gdw_navigator();
  
 private:
  void event_handler  (wxCommandEvent& event);
};

#endif
