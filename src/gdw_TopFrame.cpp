///
/// \class TopFrame gdw_TopFrame.h
///
/// \brief The top wxWidgets frame in the GenDat Explorer application
///
/// This class creates the wxWidgets frame at the top of the display hierarchy, places the main
/// menu on that frame, and handles all display events produced when a user clicks on one
/// of these menu items. The top frame also holds all of the other display elements used in
/// the rest of the program.
/// 

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/notebook.h>

#include "database.h"
#include "gdw_TopFrame.h"
#include "gdw_edit.h"
#include "gdw_dialog.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// The constructor creates the top frame, populates main menu, and binds the event handler
/// to appropriate menu items
///
////////////////////////////////////////////////////////////////////////////////////////////////////



TopFrame::TopFrame(const wxString& title, const wxPoint& pos, const wxSize& size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
  std::cout << "TopFrame Constructor Start" << std::endl;

  // Bind the event handler to the IDs used in this class.
  
  Bind(wxEVT_COMMAND_MENU_SELECTED, &TopFrame::event_handler, this, wxID_EXIT);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &TopFrame::event_handler, this, wxID_ABOUT);
  Bind(wxEVT_COMMAND_MENU_SELECTED, &TopFrame::event_handler, this, ID_first, ID_last);

  // Create a menu bar and then attach the menu items
  
  wxMenuBar *menuBar = new wxMenuBar;
  
  // File Menu
  //
  // Note that the & character creates an accelerator key. The character that
  // follows the & is underlined. This way the menu is accessible via the Alt+F
  // shortcut, while Alt+F C would get to the "DB Connect" command.

  wxMenu *menuFile = new wxMenu;
  menuBar->Append(menuFile, "&File");
  menuFile->Append(ID_Connect, "DB &Connect");
  menuFile->Append(ID_Disconnect, "DB &Disconnect");
  menuFile->Append(ID_DeletePage, "Delete Current Page");
  menuFile->AppendSeparator();
  menuFile->Append(wxID_EXIT);

  // Tools Menu

  wxMenu *menuTools = new wxMenu;
  menuBar->Append(menuTools, "&Tools");
  menuTools->Append(ID_Edit, "&Edit");

  // Help Menu

  wxMenu *menuHelp = new wxMenu;
  menuBar->Append(menuHelp, "&Help");
  menuHelp->Append(ID_ShowLogWin, "Show Log Window");
  menuHelp->Append(wxID_ABOUT);

  // Make the menu bar visible.

  SetMenuBar(menuBar);

  // Create the status bar at the bottom

  CreateStatusBar();

  // Create a top-level panel to hold all the contents of the frame
  
  wxPanel* top_panel = new wxPanel(this, wxID_ANY);

  // Create the wxNotebook widget
  
  notebook = new wxNotebook(top_panel, wxID_ANY);
  
  // Add some pages to the wxNotebook widget
  
  notebook->AddPage(new gdw_edit(notebook), L"Tab 1a");

  notebook->AddPage(new gdw_edit(notebook), L"Tab 1b");

  notebook->AddPage(new gdw_edit(notebook), L"Tab 1c");

  wxTextCtrl* textCtrl1 = new wxTextCtrl(notebook, wxID_ANY, L"Tab 1 Contents");
  notebook->AddPage(textCtrl1, L"Tab 1");
  
  wxTextCtrl* textCtrl2 = new wxTextCtrl(notebook, wxID_ANY, L"Tab 2 Contents");
  notebook->AddPage(textCtrl2, L"Tab 2");
  
  // Set up the sizer for the panel
  
  wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
  panelSizer->Add(notebook, 1, wxEXPAND);
  top_panel->SetSizer(panelSizer);

  std::cout << "TopFrame Constructor End" << std::endl;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Event handler
///
/// This member function handles all of the wxWidgets events that are generated by TopFrame.
///
////////////////////////////////////////////////////////////////////////////////////////////////////


void TopFrame::event_handler (wxCommandEvent& event)
{
  std::cout << "TopFrame::event_handler: Start" << std::endl;

  int event_id = event.GetId();

  switch (event_id)
    {
    case ID_Connect:
      {
	gdw_db_connect db_connect(&gendat_db);
	int return_code = db_connect.ShowModal();
	if (return_code==1) SetStatusText("Connected to database");
      }
      break;
      
    case ID_Disconnect:
      gendat_db.disconnect();
      SetStatusText("No database connection");
      break;

    case ID_DeletePage:
      notebook->DeletePage (notebook->GetSelection());
      break;
      
    case ID_Edit:
      notebook->AddPage(new gdw_edit(notebook), L"Tab 1");
      break;

    case ID_ShowLogWin:

      // Show log messages in a separate window.
      
      new wxLogWindow(this, wxS("Log messages"), true, false);
      break;
      
    case wxID_ABOUT:

      // Show the "About" page.
      
      wxMessageBox(PROG_VERSION, "About GenDat Explorer", wxOK | wxICON_INFORMATION);
      break;
      
    case wxID_EXIT:

      // Exit the program.
      
      Close(true);
      break;
    }
}


