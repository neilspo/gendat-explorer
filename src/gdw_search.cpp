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
/// \param [in]   parent        pointer to the parent window
/// \param [in]   db            pointer to database connection object
/// \param [in]   source_list   object containing the GenDat source definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_search::gdw_search(wxWindow* parent, database* db, const db_map& source_list) :
    gdw_panel(parent), my_source_list(source_list)
{
    my_db             = db;
    unsaved_data_flag = false;

    // Get a unique event identifier and bind it to the event handler, which is inherited from
    // the base class.

    id_mgr.reserve(1);
    id_text_event = id_mgr.alloc_id();

    Bind (wxEVT_TEXT, &gdw_search::event_handler, this, id_text_event);
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
    wx_community  = field_group.add_field ("Community",      "");
    wx_county     = field_group.add_field ("County",         "");

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

    std::string surname    (wx_surname->GetValue());
    std::string given_name (wx_given_name->GetValue());
    std::string community  (wx_community->GetValue());
    std::string county     (wx_county->GetValue());

    int num_sources = my_source_list.num_sources();
    for (int i=0; i<num_sources; i++)
    {
        std::cout << "Number of fields: " << my_source_list.num_fields(i) << std::endl;
        int num_fields = my_source_list.num_fields(i);
        for (int j=0; j<num_fields; j++)
        {
            std::cout << "Code: " << my_source_list.fld_code(i,j) << std::endl;
        }
    }




}
