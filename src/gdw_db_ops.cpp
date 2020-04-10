///
/// \class gdw_db_ops gdw_db_ops.h
///
/// \brief Provides the database operations.
///
/// This class ...
///

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/splitter.h>
#include <wx/grid.h>

#include <string>
#include <iostream>

#include "gdw_db_ops.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param [in]   parent        pointer to the parent window
/// \param [in]   db            pointer to database connection object
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_db_ops::gdw_db_ops(wxWindow* parent, database* db) : gdw_panel_lr2(parent)
{
   wxLogMessage("gdw_db_ops Constructor: Start");

    my_db = db;

    // Get a unique identifier to use when creating the list box.

    id_mgr.reserve(1);
    list_box_id = id_mgr.alloc_id();

    // Bind required event(s) to the event handler, which is inherited from
    // the base class.

    Bind (wxEVT_LISTBOX, &gdw_db_ops::event_handler, this, list_box_id);
    //wx_event_bind(wxEVT_LISTBOX, list_box_id);

}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Destructor
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_db_ops::~gdw_db_ops()
{
    wxLogMessage("gdw_db_ops Destructor: Start");

}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Determine if page has unsaved data
///
/// This member function overloads a virtual function in the base class, if required.
///
/// It determines if the page holds data that have not been saved in
/// the database. Deletion of the page would cause these data be lost.
///
/// \return   true if the page contains unsaved data, false otherwise
///
////////////////////////////////////////////////////////////////////////////////////////////////////

bool gdw_db_ops::has_unsaved_data()
{
    return unsaved_data_flag;
}





int gdw_db_ops::left_side_wx_event_type ()
{
    return wxEVT_LISTBOX;
}






void gdw_db_ops::draw_left_side(wxPanel *parent)
{
    std::string query = "SHOW TABLES";

    // Execute a database query and get the resulting row set.

    db_row_set row_set;
    my_db->execute(query, row_set);

    // process the data.

    wxArrayString table_names;
    std::string   data;

    for (unsigned int row = 0; row < row_set.num_rows(); row++)
    {
        row_set.get_data(row, 0, data);
        table_names.Add(data);
    }

    // Create a list box.

    list_box = new wxListBox (parent,
                              list_box_id,
                              wxDefaultPosition,
                              wxDefaultSize,
                              table_names,
                              wxLB_SINGLE,
                              wxDefaultValidator,
                              "Table Names");

    // Put it in a box sizer.

    wxBoxSizer *sizer1 = new wxBoxSizer(wxVERTICAL);
    sizer1->Add(list_box, 1, wxEXPAND);

    parent->SetSizer(sizer1);

}




void gdw_db_ops::draw_right_side_top(wxPanel *parent)
{
    std::string table_name;
    std::string query;
    db_row_set  row_set;

    // Get the selected table name.

    table_name = list_box->GetString(list_box->GetSelection());


    wxTextCtrl *txt = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition,
                                     wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);

    (*txt)<< "Table Name:     " << table_name << "\n\n";

    // Put the text in a sizer and add it to the text panel.

    wxBoxSizer *TextSizer = new wxBoxSizer(wxVERTICAL);

    TextSizer->Add(txt, 1, wxEXPAND, 0);
    parent->SetSizer(TextSizer);


}



void gdw_db_ops::draw_right_side_bottom (wxPanel *parent)
{
    std::string table_name;
    std::string query;
    db_row_set  row_set;


    // Get the selected table name.

    table_name = list_box->GetString(list_box->GetSelection());

    // Get the table description from the database server.

    query = "DESCRIBE " + table_name;
    my_db->execute(query, row_set);

    // Get the number of rows and columns in the row set.

    unsigned int num_rows = row_set.num_rows();
    unsigned int num_cols = row_set.num_cols();

    // Create the data display table.

    wxGrid* grid = new wxGrid(parent, wxID_ANY);
    grid->CreateGrid(num_rows, num_cols);
    grid->EnableEditing(false);
    grid->HideRowLabels();

    // Fill in the display table.

    for (unsigned int col = 0; col < num_cols; col++)
    {
        std::string data;

        // Set the table column label to the database column name.

        grid->SetColLabelValue(col, row_set.col_name(col));

        // Copy the data elements to the table.

        for (unsigned int row = 0; row < num_rows; row++)
        {
            if (row_set.get_data(row, col, data))
            {
                grid->SetCellValue(row, col, data);
            }
        }
    }

    grid->AutoSize();

    wxBoxSizer *DataSizer = new wxBoxSizer(wxHORIZONTAL);
    DataSizer->Add(grid, 5, wxEXPAND, 0);
    parent->SetSizer(DataSizer);
}

