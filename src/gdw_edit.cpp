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
        std::cout << "gdw_edit Constructor: Start" << std::endl;
  
        my_db = db;

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

        grid = new wxGrid(this, wxID_ANY, wxPoint(-1, -1), this->GetSize());

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
