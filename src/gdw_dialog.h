#ifndef GDW_DIALOG_H
#define GDW_DIALOG_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dialog.h>

class gdw_db_connect : public wxDialog
{
public:
  gdw_db_connect();
  ~gdw_db_connect();
  
private:
  virtual void event_handler  (wxCommandEvent& event);

  wxTextCtrl* form_input (wxWindow *parent, wxBoxSizer *vbox,
			  const std::string label, const std::string default_value);

  wxTextCtrl* wx_hostname;
  wxTextCtrl* wx_username;
  wxTextCtrl* wx_password;
};

#endif
