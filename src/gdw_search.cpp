///
/// \class gdw_search gdw_search.h
///
/// \brief Provides the GenDat search function.
///
/// This class ...
///

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include "gdw_search.h"
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

gdw_search::gdw_search(wxWindow* parent, database* db) : gdw_panel(parent)
{
    my_db             = db;
    unsaved_data_flag = false;

    // Get a unique event identifier and bind it to the event handler, which is inherited from
    // the base class.

    id_mgr.reserve(1);
    id_text_event = id_mgr.alloc_id();

    Bind (wxEVT_TEXT, &gdw_search::event_handler, this, id_text_event);

    std::cout << "gdw_search Constructor: " << id_mgr.first_id() << ":" << id_mgr.last_id() << std::endl;
}

gdw_search::~gdw_search()
{
    std::cout << "gdw_search Destructor: Start" << std::endl;

}



void gdw_search::process_window_draw()
{
    std::cout << "****** gdw_search::process_window_draw" << std::endl;

    // Use a vertical box sizer to layout the parts of the dialog.

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    // Add the user input fields.

    gdw_field_group field_group(this, vbox, id_text_event, 150, 300);
    wx_surname    = field_group.add_field ("Surname",        "");
    wx_given_name = field_group.add_field ("Given Names(s)", "");

    // Now display everything.

    this->SetSizer(vbox);
    SetClientSize(this->GetBestSize());

    // Indicate that there is currently no unsaved user input data.

    unsaved_data_flag = false;
}


void gdw_search::process_window_events (wxEvent* event)
{
    unsigned int event_id = event->GetId();

    if (event_id == id_text_event)
    {
        unsaved_data_flag = true;
    }
}


bool gdw_search::has_unsaved_data()
{
    return unsaved_data_flag;
}


void gdw_search::process_execute()
{
    std::cout << "****** gdw_search::process_execute" << std::endl;
}
