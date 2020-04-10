///
/// \class gdw_panel_lr2 gdw_panel_lr2.h
///
/// \brief Provides ...
///
/// This class ...
///

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/splitter.h>

#include "gdw_panel_lr2.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param [in]   parent        pointer to the parent window
/// \param [in]   db            pointer to database connection object
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_panel_lr2::gdw_panel_lr2(wxWindow* parent) : gdw_panel(parent)
{
    wxLogMessage("gdw_panel_lr2 Constructor: Start");
}




////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_panel_lr2::~gdw_panel_lr2()
{
    wxLogMessage("gdw_panel_lr2 Destructor: Start");

}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Process a window draw request
///
/// This member function overloads a pure virtual function in the base class and must be defined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel_lr2::process_window_draw()
{
    wxLogMessage("****** gdw_panel_lr2::process_window_draw");

    // Create a new box sizer to hold the splitter window.

    wxBoxSizer *sizermain = new wxBoxSizer(wxVERTICAL);

    // Create the splitter window and add it to the sizer.

    wxSplitterWindow *splittermain = new wxSplitterWindow(this, wxID_ANY);
    splittermain->SetSashGravity(0.3);
    splittermain->SetMinimumPaneSize(20);
    sizermain->Add(splittermain, 1,wxEXPAND,0);

    // Left side

    wxPanel *left_side = new wxPanel(splittermain, wxID_ANY);
    draw_left_side (left_side);

    //Right side

    right_side = new wxPanel(splittermain, wxID_ANY);




    splittermain->SplitVertically(left_side, right_side);

    this->SetSizer(sizermain);
    sizermain->SetSizeHints(this);
    GetGrandParent()->Layout();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Process a window event
///
/// This member function overloads a pure virtual function in the base class and must be defined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel_lr2::process_window_events (wxEvent* event)
{
    wxLogMessage("****** gdw_panel_lr2::process_window_events");

    // Clear the right side display panel.

    right_side->DestroyChildren();

    // Put everything in a vertical box sizer.

    wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);

    // Add a panel for descriptive text and another one for tabular data
    // about the GenDat source database fields.

    wxPanel *TextPanel = new wxPanel (right_side, wxID_ANY, wxDefaultPosition, wxSize(-1, 100));
    wxPanel *DataPanel = new wxPanel (right_side, wxID_ANY);

    MainSizer->Add(TextPanel, 1, wxEXPAND, 0);
    MainSizer->Add(DataPanel, 1, wxEXPAND, 0);

    draw_right_side_top    (TextPanel);

    draw_right_side_bottom (DataPanel);

    right_side->SetSizer(MainSizer);
    right_side->Layout();
}




////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Process an execute function request.
///
/// This member function overloads a pure virtual function in the base class and must be defined.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel_lr2::process_execute()
{
    wxLogMessage("****** gdw_panel_lr2::process_execute");
}
