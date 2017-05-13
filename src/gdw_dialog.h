///
/// \file
///



#ifndef GDW_DIALOG_H
#define GDW_DIALOG_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dialog.h>

#include "database.h"

class gdw_db_connect : public wxDialog
{
public:
    gdw_db_connect(database* db);
    ~gdw_db_connect();

private:
    virtual void event_handler  (wxCommandEvent& event);



    wxTextCtrl* wx_hostname;
    wxTextCtrl* wx_username;
    wxTextCtrl* wx_password;
    database*   my_db;
};

#endif
