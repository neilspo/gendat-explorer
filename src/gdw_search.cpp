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

#include <wx/splitter.h>

#include <string>
#include "gdw_search.h"
#include "gdw_field_group.h"

#include "gde_search_map.h"


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

gdw_search::gdw_search(wxWindow* parent, database* db, const gde_source_map& source_map) :
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
    wxLogMessage("gdw_search Destructor: Start");

}



void gdw_search::process_window_draw()
{
    wxLogMessage("****** gdw_search::process_window_draw");

    // Create a new box sizer to hold everything.

    wxBoxSizer *sizermain = new wxBoxSizer(wxVERTICAL);

    // Create the splitter window and add it to the sizer.

    wxSplitterWindow *splittermain = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_BORDER);
    splittermain->SetSize(GetClientSize());
    splittermain->SetSashGravity(0.5);



    splittermain->SetMinimumPaneSize(200);
    sizermain->Add(splittermain, 1, wxEXPAND, 0);

    wxLogMessage("GetSashSize %d", splittermain->GetSashSize());

    // Left side

    wxPanel *left_side = new wxPanel(splittermain, wxID_ANY);

    wxTextCtrl *txt = new wxTextCtrl(left_side, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
    (*txt)<< "Left Side" << "\n\n";
    wxBoxSizer *sizer1 = new wxBoxSizer(wxVERTICAL);
    sizer1->Add(txt, 1, wxEXPAND, 10);
    left_side->SetSizer(sizer1);

    //Right side

    wxScrolledWindow *right_side = new wxScrolledWindow(splittermain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED);

    draw_search_form(right_side);

    right_side->FitInside(); // ask the sizer about the needed size
    right_side->SetScrollRate(5, 5);





    splittermain->SplitVertically(left_side, right_side);

    this->SetSizerAndFit(sizermain);
    sizermain->SetSizeHints(this);
    GetGrandParent()->Layout();









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

    gde_search_map my_search_map(my_source_map);

    // Include birth and marriage records in the search.

    my_search_map.add_source (gde_data_tag::BIRT);
    my_search_map.add_source (gde_data_tag::MARR);

    // Now select the fields that will be included in the search.

    my_search_map.req_field(gde_relation::UNDEFINED,
                            gde_data_tag::UNDEFINED,
                            gde_data_tag::SURN,
                            gde_data_tag::UNDEFINED);

    my_search_map.opt_field(gde_relation::UNDEFINED,
                            gde_data_tag::UNDEFINED,
                            gde_data_tag::GIVN,
                            gde_data_tag::UNDEFINED);

    my_search_map.opt_field(gde_relation::UNDEFINED,
                            gde_data_tag::UNDEFINED,
                            gde_data_tag::PLAC,
                            gde_data_tag::COMMUNITY);

    my_search_map.opt_field(gde_relation::UNDEFINED,
                            gde_data_tag::UNDEFINED,
                            gde_data_tag::PLAC,
                            gde_data_tag::COUNTY);





}


void::gdw_search::draw_search_form(wxPanel *parent)
{
    // Use a vertical box sizer to layout the parts of the dialog.

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    // Add the user input fields.

    gdw_field_group field_group(parent, vbox, id_text_event, 150, 300);
    wx_surname    = field_group.add_field ("Surname",        "");
    wx_given_name = field_group.add_field ("Given Names(s)", "");
    wx_community  = field_group.add_field ("Community",      "");
    wx_county     = field_group.add_field ("County",         "");

    parent->SetSizer(vbox);
    vbox->SetSizeHints(parent);
}
