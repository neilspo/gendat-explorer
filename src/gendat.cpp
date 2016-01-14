#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/treectrl.h>

#include <string>
#include "database.h"



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
	void OnExit         (wxCommandEvent& event);
	void OnAbout        (wxCommandEvent& event);

	void OnConnect      (wxCommandEvent& event);
	void OnDisconnect   (wxCommandEvent& event);
	void OnShowLogWin   (wxCommandEvent& event);
	void OnEdit         (wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

	wxPanel      *main_panel;
	database     gendat_db;

};


// ============================================================================
// Event table.
// ============================================================================

enum
{
	ID_Connect = 1,
	ID_Disconnect,
	ID_ShowLogWin,
	ID_Edit
};

wxBEGIN_EVENT_TABLE(TopFrame, wxFrame)

EVT_MENU(wxID_EXIT,      TopFrame::OnExit)
EVT_MENU(wxID_ABOUT,     TopFrame::OnAbout)

EVT_MENU(ID_Connect,     TopFrame::OnConnect)
EVT_MENU(ID_Disconnect,  TopFrame::OnDisconnect)
EVT_MENU(ID_ShowLogWin,  TopFrame::OnShowLogWin)
EVT_MENU(ID_Edit,        TopFrame::OnEdit)

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

	// Tools Menu

	wxMenu *menuTools = new wxMenu;
	menuTools->Append(ID_Edit, "&Edit");

	// Help Menu

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(ID_ShowLogWin, "Show Log Window");
	menuHelp->Append(wxID_ABOUT);


	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile,    "&File");
	menuBar->Append(menuTools,   "&Tools");
	menuBar->Append(menuHelp,    "&Help");
	SetMenuBar(menuBar);

	// Create the status bar at the bottom

	CreateStatusBar();

	// Create the main data display area.

	main_panel = new wxPanel(this);

	// put out a bit is text to test the panel.

	wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(new wxStaticText(main_panel, wxID_ANY, wxT("Startup Window")), 0, wxALL, 5);

	main_panel->SetSizer(sizer);
	std::cout << "test";
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
	wxMessageBox("Genealogical Data Miner\n\nVersion 0.1",
		"About GenDat Miner", wxOK | wxICON_INFORMATION);
}


// Connect to database.

void TopFrame::OnConnect(wxCommandEvent& event)
{
	std::string host    = "127.0.0.1";
	//std::string host = "server";
	std::string user    = "test_RW";
	std::string passwd  = "1234";
	std::string db_name = "zzz";
	
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



// Event handler for menu item - Show Log Window

void TopFrame::OnShowLogWin(wxCommandEvent& event)
{
	new wxLogWindow(this, wxS("Log messages"), true, false);
}



// Event handler for menu item - Tools > Edit

void TopFrame::OnEdit(wxCommandEvent& event)
{
	wxPanel* parent = main_panel;
	
	// Clear the parent window.

	parent->DestroyChildren();

	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

	wxTreeCtrl *tree = new wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize);

	sizer->Add(new wxStaticText(parent, wxID_ANY, wxT("Test Window")), 0, wxALL, 5);
	sizer->Add(tree, 1, wxEXPAND);

	tree->SetWindowStyle(wxTR_HIDE_ROOT); //hides the real root node to have many roots

	wxTreeItemId root = tree->AddRoot("invisible root");//this root will be invisible
	wxTreeItemId r1   = tree->AppendItem(root, "1871 Census");
	wxTreeItemId r2   = tree->AppendItem(root, "1881 Census");
	tree->AppendItem(r1, "Node1");
	tree->AppendItem(r1, "Node2");
	tree->AppendItem(r2, "Node3");

	parent->SetSizer(sizer);
	SetClientSize(parent->GetBestSize());
}