//
// This program tests a simple wxWidgets GUI that uses the database access code.
//
// Usage: ./test_db_with_wxWidgets Host, User, Password
//


#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/colour.h>
#include <string>
#include <sstream>
#include <iostream>
#include "database.h"
#include "db_row_set_w.h"

// ============================================================================
// Class definitions.
// ============================================================================

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int  OnExit();
};

class TopFrame : public wxFrame
{
public:
	TopFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
	void OnConnect      (wxCommandEvent& event);
	void OnDisconnect   (wxCommandEvent& event);
	void OnExit         (wxCommandEvent& event);
	void OnShowLogWin   (wxCommandEvent& event);
	void OnAbout        (wxCommandEvent& event);
	void On_1871_Census (wxCommandEvent& event);

	void OnCellValueChanged (wxGridEvent&);
	void OnGridButton_Save  (wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

	database     gendat_db;
	db_row_set_w row_set;
	wxPanel      *main_panel;
	wxGrid       *grid;
};


// ============================================================================
// Event table.
// ============================================================================

enum
{
	ID_Connect = 1,
	ID_Disconnect,
	ID_ShowLogWin,
	ID_1871_Census,

	ID_GridButton_Save
};

wxBEGIN_EVENT_TABLE(TopFrame, wxFrame)
EVT_MENU(ID_Connect,     TopFrame::OnConnect)
EVT_MENU(ID_Disconnect,  TopFrame::OnDisconnect)
EVT_MENU(ID_ShowLogWin,  TopFrame::OnShowLogWin)
EVT_MENU(wxID_EXIT,      TopFrame::OnExit)
EVT_MENU(wxID_ABOUT,     TopFrame::OnAbout)
EVT_MENU(ID_1871_Census, TopFrame::On_1871_Census)

EVT_GRID_CELL_CHANGED(TopFrame::OnCellValueChanged)
EVT_BUTTON(ID_GridButton_Save, TopFrame::OnGridButton_Save)

wxEND_EVENT_TABLE()



// OnInit() serves as the main procedure for wxWidgets programs.


wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	TopFrame *frame = new TopFrame("GenDat Miner", wxPoint(-1, -1), wxSize(1000, 500));
	frame->Show(true);
	return true;
}

int MyApp::OnExit()
{
	return 0;
}



/// Create the top frame.


TopFrame::TopFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
{

	//	File Menu

	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Connect, "&DB Connect");
	menuFile->Append(ID_Disconnect, "&DB Disconnect");
	menuFile->AppendSeparator();
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	// Souurces Menu

	wxMenu *menuSources = new wxMenu;
	menuSources->Append(ID_1871_Census, "1871 Census");

	// Help Menu

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(ID_ShowLogWin, "Show Log Window");
	menuHelp->Append(wxID_ABOUT);


	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile,    "&File");
	menuBar->Append(menuSources, "&Sources");
	menuBar->Append(menuHelp,    "&Help");
	SetMenuBar(menuBar);

	// Create the status bar at the bottom

	CreateStatusBar();

	// Create the main data display area.

	main_panel = new wxPanel(this);

	// put out a bit is text to test the panel.

	wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Label1")), 0, wxALL, 5);
	sizer->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Label2")), 0, wxALL, 5);

	main_panel->SetSizer(sizer);
}



// ============================================================================
// Event handlers
// ============================================================================


void TopFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void TopFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("This is a wxWidgets' Hello world sample",
		"About Hello World", wxOK | wxICON_INFORMATION);
}


// Connect to database.

void TopFrame::OnConnect(wxCommandEvent& event)
{
	if (wxGetApp().argc!=4)
	{
		std::cerr << "Usage: " << wxGetApp().argv[0] << " Host, User, Password" << std::endl;
		return;
	}

	std::string host    = std::string(wxGetApp().argv[1]);
	std::string user    = std::string(wxGetApp().argv[2]);
	std::string passwd  = std::string(wxGetApp().argv[3]);
	std::string db_name = "gendat";
	
	try
	{
		// Connect to the database.

		gendat_db.connect(host, user, passwd, db_name);

		// Change the window status message.

		SetStatusText("Connected to database");
	}
	catch (std::runtime_error& exception)
	{
		wxLogMessage(exception.what());
	}
}



// Disconnect from database.

void TopFrame::OnDisconnect(wxCommandEvent& event)
{
	gendat_db.disconnect();
	SetStatusText("No database connection");
}



/// Event handler for menu item - Show Log Window

void TopFrame::OnShowLogWin(wxCommandEvent& event)
{
	new wxLogWindow(this, wxS("Log messages"), true, false);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// Event handlers related to drawing and managing an editable grid on main_panel.
///
////////////////////////////////////////////////////////////////////////////////////////////////////


// Event handler for menu item - 1871 Census

void TopFrame::On_1871_Census(wxCommandEvent& event)
{
	unsigned int num_rows;
	unsigned int num_cols;
	
	//std::string query = "SELECT id, t_int, t_int + 2 from zzz";
	//std::string query = "SELECT * FROM zzz";
	std::string query = "SELECT * FROM 1871_census_data WHERE (district_id=197 AND sub_district_id='l' AND page=1)";
	//std::string query = "DESCRIBE 1871_census_data";

	try
	{
		// Execute a database query and get the resulting row set.

		gendat_db.execute(query, row_set);

		// Get the number of rows and columns in the row set.

		num_rows = row_set.num_rows();
		num_cols = row_set.num_cols();

		// Clear the main data display panel.

		main_panel->DestroyChildren();

		// Create a sizer to hold the data table.

		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		// Add some buttons below the data display table.

		sizer->Add(new wxButton(main_panel, ID_GridButton_Save, "Save"), 0, 0, 0);

		// Create the data display table.

		grid = new wxGrid(main_panel, wxID_ANY, wxPoint(-1, -1), main_panel->GetSize());

		grid->CreateGrid(num_rows, num_cols);
		sizer->Add(grid, 1, wxEXPAND, 10);

		// Fill in the display table.

		std::string data;
		for (unsigned int col = 0; col < num_cols; col++)
		{

			// Set the table column label to the database column name.

			grid->SetColLabelValue(col, row_set.col_name(col));

			// Copy the data elements to the table.

			for (unsigned int row = 0; row < num_rows; row++)
			{
				if (row_set.get_data(row, col, data))
				{
					grid->SetCellValue(row, col, data);
				}
				else
				{
					grid->SetCellValue(row, col, "(NULL)");
					grid->SetCellBackgroundColour(row, col, *wxCYAN);
				}
				
			}

		}

		// Finish the panel.

		main_panel->SetSizer(sizer);
		SetClientSize(main_panel->GetBestSize());
	}
	catch (std::runtime_error& exception)
	{
		wxLogMessage(exception.what());
	}
	catch (std::logic_error& exception)
	{
		wxLogMessage("Logic Error: %s", exception.what());
	}
}



// Event handler called whenever a grid element is changed.

void TopFrame::OnCellValueChanged(wxGridEvent& ev)
{
	int row = ev.GetRow();
	int col = ev.GetCol();
	std::string data;
	std::string error_msg;

	data = grid->GetCellValue(row, col);
	if (row_set.save_data(row, col, data, error_msg))
	{
		grid->SetCellBackgroundColour(row, col, *wxYELLOW);
	}
	else
	{
		wxLogMessage(error_msg.c_str());
		grid->SetCellValue(row, col, ev.GetString());

	}
	
	ev.Skip();
}


// Event handler called when grid button Save is pressed.

void TopFrame::OnGridButton_Save(wxCommandEvent& ev)
{
	wxLogMessage("Save button pressed (this program does not actually change the database)");
}
