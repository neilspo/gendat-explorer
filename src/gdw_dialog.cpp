///
/// \class gdw_db_connect gdw_dialog.h
///
/// \brief Popup window that obtains database login information from the user
///
/// This class handles ...
///

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include <stdexcept>

#include "gdw_dialog.h"
#include "gdw_field_group.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// The constructor creates the popup window and waits for user input.
///
////////////////////////////////////////////////////////////////////////////////////////////////////



gdw_db_connect::gdw_db_connect(database* db) : wxDialog(NULL, wxID_ANY, "Database Connect")
{
  // Keep a pointer to the database object.

  my_db = db;

  // Put everything in a panel.

  wxPanel *panel = new wxPanel(this, wxID_ANY);

  // Use a vertical box sizer to layout the parts of the dialog.

  wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

  // Add the user input fields.

  gdw_field_group field_group(panel, vbox, wxID_ANY, 100, -1);

  wx_hostname = field_group.add_field ("Host Name", "apollo");
  wx_username = field_group.add_field ("User Name", "test_RO");
  wx_password = field_group.add_field ("Password",  "");

  // Add "Cancel" and "Apply" buttons.

  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
  hbox->Add(new wxButton(panel, wxID_CANCEL));
  hbox->Add(new wxButton(panel, wxID_APPLY));
  vbox->Add(hbox, 0, wxALIGN_RIGHT | wxTOP | wxRIGHT | wxBOTTOM, 10);

  // Bind the event handler, which will only execute after the "Apply" button is pressed.

  Bind(wxEVT_BUTTON, &gdw_db_connect::event_handler, this, wxID_APPLY);

  // Now display everything.

  panel->SetSizer(vbox);
  Centre();
}


gdw_db_connect::~gdw_db_connect()
{
  std::cout << "gdw_db_connect Destructor Start" << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Event handler
///
/// This member function handles all of the wxWidgets events that are generated by TopFrame.
///
////////////////////////////////////////////////////////////////////////////////////////////////////


void gdw_db_connect::event_handler (wxCommandEvent& event)
{
  std::cout << "db_connect::event_handler: Start" << std::endl;

  std::string host   (wx_hostname->GetValue());
  std::string user   (wx_username->GetValue());
  std::string passwd (wx_password->GetValue());
  std::string db_name("zzz");

  // Try to connect to the database. If it worked, then kill the dialog window.
  // Otherwise, report the error and leave the dialog window open.

  try
    {
      my_db->connect(host, user, passwd, db_name);
      EndModal(1);
    }
  catch (std::runtime_error& exception)
    {
      wxLogMessage(exception.what());
    }
}
