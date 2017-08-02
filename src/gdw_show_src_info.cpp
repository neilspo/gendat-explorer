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
    my_db             = db;
    unsaved_data_flag = false;

    // Get a unique event identifier and bind it to the event handler, which is inherited from
    // the base class.

    id_mgr.reserve(1);
    id_text_event = id_mgr.alloc_id();

    Bind (wxEVT_TEXT, &gdw_show_src_info::event_handler, this, id_text_event);

    std::cout << "gdw_show_src_info Constructor: " << id_mgr.first_id() << ":" << id_mgr.last_id() << std::endl;
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
    splittermain->SetMinimumPaneSize(20); // Smalest size the
    sizermain->Add(splittermain, 1,wxEXPAND,0);

    wxPanel *pnl1 = new wxPanel(splittermain, wxID_ANY);

    wxBoxSizer *txt1sizer = new wxBoxSizer(wxVERTICAL);
    wxTextCtrl *txt1 = new wxTextCtrl(pnl1, wxID_ANY, "Left Side");
    txt1sizer->Add(txt1, 1,wxEXPAND,0);
    pnl1->SetSizer(txt1sizer);

    wxPanel *pnl2 = new wxPanel(splittermain, wxID_ANY);

    wxBoxSizer *txt2sizer = new wxBoxSizer(wxVERTICAL);
    wxTextCtrl *txt2 = new wxTextCtrl(pnl2, wxID_ANY, "Right Side");
    txt2sizer->Add(txt2, 1,wxEXPAND,0);
    pnl2->SetSizer(txt2sizer);

    splittermain->SplitVertically(pnl1, pnl2);

    this->SetSizer(sizermain);
    sizermain->SetSizeHints(this);
}


void gdw_show_src_info::process_window_events (wxEvent* event)
{
    unsigned int event_id = event->GetId();

    if (event_id == id_text_event)
    {
        unsaved_data_flag = true;
    }
}


bool gdw_show_src_info::has_unsaved_data()
{
    return unsaved_data_flag;
}


void gdw_show_src_info::process_execute()
{
    std::cout << "****** gdw_show_src_info::process_execute" << std::endl;
}
