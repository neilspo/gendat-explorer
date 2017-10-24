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

#include "gde_source_map.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// The constructor
///
/// \param [in]   parent        pointer to the parent window
/// \param [in]   db            pointer to database connection object
/// \param [in]   source_map    object containing the GenDat source definitions
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_search::gdw_search(wxWindow* parent, database* db, const db_map& source_map) :
    gdw_panel(parent), my_source_map(source_map)
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



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Process the execute command
///
////////////////////////////////////////////////////////////////////////////////////////////////////


void gdw_search::process_execute()
{
    // Find out what to search for.

    std::string surname    (wx_surname->GetValue());
    std::string given_name (wx_given_name->GetValue());
    std::string community  (wx_community->GetValue());
    std::string county     (wx_county->GetValue());

    //*********************************************************************************************
    // This next line is here for test purposes only.
    //*********************************************************************************************

    gde_source_map test(my_source_map);

    // Find all of the GenDat sources that have a "surname" field.

    int num_sources = my_source_map.num_sources();
    for (int i=0; i<num_sources; i++)
    {
        for (int j=0; j<my_source_map.num_fields(i); j++)
        {
            if (test.fact_type(i,j) == gde_data_tag::SURN)
            {
                std::cout << my_source_map.src_db_table(i) << ", " << my_source_map.fld_db_name(i,j) << std::endl;
            }
        }
    }




}
