///
/// \class gdw_show_src_info gdw_show_src_info.h
///
/// \brief Show information about GenDat sources.
///
/// This class ...
///

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/splitter.h>

#include <string>
#include "gdw_show_src_info.h"
#include "gdw_field_group.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// The constructor
///
/// \param [in]   parent   pointer to the parent window
/// \param [in]   db       pointer to database connection object
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_show_src_info::gdw_show_src_info(wxWindow* parent, database* db) : gdw_panel(parent)
{
    my_db = db;

    // Get a unique identifier to use when creating the tree control.

    id_mgr.reserve(1);
    tree_id = id_mgr.alloc_id();

    // Bind required event(s) to the event handler, which is inherited from
    // the base class.

    Bind (wxEVT_TREE_SEL_CHANGED, &gdw_show_src_info::event_handler, this, tree_id);

}




gdw_show_src_info::~gdw_show_src_info()
{
    std::cout << "gdw_show_src_info Destructor: Start" << std::endl;

}



void gdw_show_src_info::process_window_draw()
{
    std::cout << "****** gdw_show_src_info::process_window_draw" << std::endl;

    // Create a new box sizer to hold the splitter window.

    wxBoxSizer *sizermain = new wxBoxSizer(wxVERTICAL);

    // Create the splitter window and add it to the sizer.

    wxSplitterWindow *splittermain = new wxSplitterWindow(this, wxID_ANY);
    splittermain->SetSashGravity(0.5);
    splittermain->SetMinimumPaneSize(20);
    sizermain->Add(splittermain, 1,wxEXPAND,0);

    // Left side

    wxPanel *pnl1 = new wxPanel(splittermain, wxID_ANY);
    draw_left_panel(pnl1);

    //Right side

    wxPanel *pnl2 = new wxPanel(splittermain, wxID_ANY);


    wxBoxSizer *txt2sizer = new wxBoxSizer(wxVERTICAL);
    wxTextCtrl *txt2 = new wxTextCtrl(pnl2, wxID_ANY, "Right Side");
    txt2sizer->Add(txt2, 1,wxEXPAND,0);
    wxTextCtrl *txt3 = new wxTextCtrl(pnl2, wxID_ANY, "Another line");
    txt2sizer->Add(txt3, 1,wxEXPAND,0);
    pnl2->SetSizer(txt2sizer);

    splittermain->SplitVertically(pnl1, pnl2);

    this->SetSizer(sizermain);
    sizermain->SetSizeHints(this);
}



void gdw_show_src_info::process_window_events (wxEvent* event)
{
    wxEventType event_type = event->GetEventType();

    if (event_type == wxEVT_TREE_SEL_CHANGED)
    {
        wxTreeItemId selected_node = tree->GetSelection();
        std::cout << "wxEVT_TREE_SEL_CHANGED " << tree->GetItemText(selected_node) << std::endl;
    }
}



void gdw_show_src_info::process_execute()
{
    std::cout << "****** gdw_show_src_info::process_execute" << std::endl;
}


void gdw_show_src_info::draw_left_panel(wxPanel *parent)
{
    //Create a tree control.

    tree = new wxTreeCtrl(parent, tree_id, wxDefaultPosition, wxDefaultSize);

    // Put it in a box sizer.

    wxBoxSizer *sizer1 = new wxBoxSizer(wxVERTICAL);
    sizer1->Add(tree, 1, wxEXPAND);

    wxTreeItemId root = tree->AddRoot("Sources");
    wxTreeItemId r1   = tree->AppendItem(root, "1871 Census");
    wxTreeItemId r2   = tree->AppendItem(root, "1881 Census");
    tree->AppendItem(r1, "Node1");
    tree->AppendItem(r1, "Node2");
    tree->AppendItem(r2, "Node3");

    parent->SetSizer(sizer1);

}
