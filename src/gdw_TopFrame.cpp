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
#include <wx/artprov.h>

#include "database.h"
#include "db_map.h"
#include "gdw_TopFrame.h"
#include "gdw_edit.h"
#include "gdw_search.h"
#include "gdw_dialog.h"
#include "gdw_show_src_info.h"



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

  //****************************************************************************
  // Bind the event handler to the IDs used in this class.
  //****************************************************************************

  Bind(wxEVT_MENU, &TopFrame::event_handler, this, wxID_EXIT);
  Bind(wxEVT_MENU, &TopFrame::event_handler, this, wxID_ABOUT);
  Bind(wxEVT_MENU, &TopFrame::event_handler, this, ID_first, ID_last);

  //****************************************************************************
  // Create a menu bar and then attach the menu items
  //****************************************************************************

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
  menuTools->Append(ID_ShowSourceInfo, "Show Source Info");
  menuTools->Append(ID_Search, "Search");

  // Help Menu

  wxMenu *menuHelp = new wxMenu;
  menuBar->Append(menuHelp, "&Help");
  menuHelp->Append(ID_ShowLogWin, "Show Log Window");
  menuHelp->Append(wxID_ABOUT);

  // Make the menu bar visible.

  SetMenuBar(menuBar);

  //****************************************************************************
  // Create the status bar at the bottom
  //****************************************************************************

  CreateStatusBar();

  //****************************************************************************
  // Create a tool bar and then attach the menu items
  //****************************************************************************

  wxToolBar *toolBar = CreateToolBar();

  wxBitmap icon_execute     = wxArtProvider::GetBitmap(wxART_GO_FORWARD,wxART_TOOLBAR);
  wxBitmap icon_page_delete = wxArtProvider::GetBitmap(wxART_CROSS_MARK,wxART_TOOLBAR);
  wxBitmap icon_page_reload = wxArtProvider::GetBitmap(wxART_UNDO,wxART_TOOLBAR);

  toolBar->AddTool(ID_DeletePage, "Delete",  icon_page_delete, "Delete Current Page");
  toolBar->AddTool(ID_ReloadPage, "Reload",  icon_page_reload, "Reload Current Page");
  toolBar->AddTool(ID_Execute,    "Execute", icon_execute,     "Execute Task");

  toolBar->Realize();

  //****************************************************************************
  // Create a top-level panel to hold the notebook and its contents
  //****************************************************************************

  wxPanel* top_panel = new wxPanel(this, wxID_ANY);

  // Create the wxNotebook widget

  notebook = new wxNotebook(top_panel, wxID_ANY);

  // Set up the sizer for the panel

  wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);
  panelSizer->Add(notebook, 1, wxEXPAND);
  top_panel->SetSizer(panelSizer);
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
            if (return_code==1)
            {
                SetStatusText("Connected to database");
                gendat_sources.load_defs(gendat_db, "z_sour", "z_sour_fldsa");
            }
        }
        break;

    case ID_Disconnect:
      gendat_db.disconnect();
      SetStatusText("No database connection");
      break;

    case ID_DeletePage:
      {
        int selected_page = notebook->GetSelection();
        if (selected_page != wxNOT_FOUND)
          notebook->DeletePage(selected_page);
      }
      break;

    case ID_ReloadPage:
      {
        gdw_panel* p_notebook_page = (gdw_panel*) notebook->GetCurrentPage();
        if (p_notebook_page != nullptr)
          p_notebook_page->page_reload();
        break;
      }

    case ID_Execute:
      {
        gdw_panel* p_notebook_page = (gdw_panel*) notebook->GetCurrentPage();
        if (p_notebook_page != nullptr)
          p_notebook_page->page_execute();
        break;
      }

    case ID_Edit:
      notebook->AddPage(new gdw_edit(notebook, &gendat_db), L"Edit", true);
      break;

    case ID_ShowSourceInfo:
      notebook->AddPage(new gdw_show_src_info(notebook, &gendat_db, gendat_sources), L"GenDat Source Info", true);
      break;

    case ID_Search:
        notebook->AddPage(new gdw_search(notebook, &gendat_db, gendat_sources), L"Search", true);
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


