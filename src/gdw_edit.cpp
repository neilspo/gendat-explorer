///
/// \class gdw_edit gdw_edit.h
///
/// \brief Provides the GenDat edit function.
///
/// This class ...
///

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//#include <wx/sizer.h>
//#include <wx/colour.h>
#include <string>


#include "gdw_edit.h"

/*

  Constructor

*/

gdw_edit::gdw_edit(wxWindow* parent, database* db) : gdw_panel(parent)
{
        my_db = db;

        // Get 2 unique event identifiers and bind them to the event handler.
        
        id_mgr.reserve(2);
        
        id_grid_event = id_mgr.alloc_id();
        id_save_event = id_mgr.alloc_id();

        Bind (wxEVT_GRID_CELL_CHANGED, &gdw_edit::event_handler, this , id_grid_event);

        std::cout << "gdw_edit Constructor: " << id_mgr.first_id() << ":" << id_mgr.last_id() << std::endl;
}

gdw_edit::~gdw_edit()
{
        std::cout << "gdw_edit Destructor: Start" << std::endl;
  
}

void gdw_edit::process_window_draw()
{
        std::cout << "****** edit::process_window_draw" << std::endl;

        unsigned int num_rows;
        unsigned int num_cols;

        std::string query = "SELECT * FROM 1871_census_data WHERE (district_id=197 AND sub_district_id='l' AND page=1)";

        // Execute a database query and get the resulting row set.

        my_db->execute(query, row_set);

        // Get the number of rows and columns in the row set.

        num_rows = row_set.num_rows();
        num_cols = row_set.num_cols();

        // Clear the main data display panel.

        this->DestroyChildren();

        // Create a sizer to hold the data table.

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

        // Add some buttons below the data display table.

        // sizer->Add(new wxButton(this, ID_GridButton_Save, "Save"), 0, 0, 0);

        // Create the data display table.

        grid = new wxGrid(this, id_grid_event, wxPoint(-1, -1), this->GetSize());

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

        this->SetSizer(sizer);
}


void gdw_edit::process_window_events (wxEvent* event)
{
        unsigned int event_id = event->GetId();
        
        std::cout << "process_window_events: " << event_id << std::endl;


        if (event_id == id_grid_event)
        {
        
                wxGridEvent* grid_event = (wxGridEvent*)event;
                int row = grid_event->GetRow();
                int col = grid_event->GetCol();

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
                        grid->SetCellValue(row, col, grid_event->GetString());

                }
        }
        else if (event_id == id_save_event)
        {
                std::cout << "save event" << std::endl;
        }


}
