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
#include <wx/grid.h>

#include <string>
#include "db_row_set.h"
#include "gdw_show_src_info.h"


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

gdw_show_src_info::gdw_show_src_info(wxWindow* parent, database* db,
                                     gendat_source_list source_list) : gdw_panel(parent)
{
    my_db          = db;
    my_source_list = source_list;

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
    splittermain->SetSashGravity(0.3);
    splittermain->SetMinimumPaneSize(20);
    sizermain->Add(splittermain, 1,wxEXPAND,0);

    // Left side

    wxPanel *pnl1 = new wxPanel(splittermain, wxID_ANY);
    draw_left_panel(pnl1);

    //Right side

    right_side = new wxPanel(splittermain, wxID_ANY);




    splittermain->SplitVertically(pnl1, right_side);

    this->SetSizer(sizermain);
    sizermain->SetSizeHints(this);
}



void gdw_show_src_info::process_window_events (wxEvent* event)
{
    wxEventType event_type = event->GetEventType();

    if (event_type == wxEVT_TREE_SEL_CHANGED)
    {
        // Get the ID of the newly selected node.

        wxTreeItemId selected_node = tree->GetSelection();

        // Try to get a pointer to the data transfer object.

        ItemData *p_node_data = (ItemData*)tree->GetItemData(selected_node);

        // If there was data transfered, then process it.

        if(p_node_data)
        {
            int source = p_node_data->GetData();

            std::string source_name        = my_source_list.get_name(source);
            std::string source_description = my_source_list.get_description(source);
            std::string source_db_table    = my_source_list.get_db_table(source);

            // Clear the main data display panel.

            right_side->DestroyChildren();

            // Put everything in a vertical box sizer.

            wxBoxSizer *MainSizer = new wxBoxSizer(wxVERTICAL);

            // Add a panel for descriptive text and another one for tabular data
            // about the GenDat source database fields.

            wxPanel *TextPanel = new wxPanel (right_side, wxID_ANY, wxDefaultPosition, wxSize(-1, 100));
            wxPanel *DataPanel = new wxPanel (right_side, wxID_ANY);

            MainSizer->Add(TextPanel, 1, wxEXPAND, 0);
            MainSizer->Add(DataPanel, 1, wxEXPAND, 0);

            // Output text information about the selected GenDat source.

            wxTextCtrl *txt = new wxTextCtrl(TextPanel, wxID_ANY, "", wxDefaultPosition,
                                             wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);

            (*txt)<< "Source Name:     " << source_name << "\n\n";
            (*txt)<< "Source Description:     " << source_description << "\n";

            // Put the text in a sizer and add it to the text panel.

            wxBoxSizer *TextSizer = new wxBoxSizer(wxVERTICAL);

            TextSizer->Add(txt, 1, wxEXPAND, 0);
            TextPanel->SetSizer(TextSizer);




            // Ask MySQL to describe the required database table.

            std::string query = "DESCRIBE " + source_db_table;
            db_row_set  row_set;
            my_db->execute(query, row_set);

            // Get the number of rows and columns in the row set.

            unsigned int num_rows = row_set.num_rows();
            unsigned int num_cols = row_set.num_cols();

            // Create the data display table.

            wxGrid* grid = new wxGrid(DataPanel,wxID_ANY);
            grid->CreateGrid(num_rows, num_cols);
            grid->EnableEditing(false);

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
            grid->AutoSize();

            wxBoxSizer *DataSizer = new wxBoxSizer(wxHORIZONTAL);

            DataSizer->Add(grid, 5, wxEXPAND, 0);
            DataPanel->SetSizer(DataSizer);








            right_side->SetSizer(MainSizer);
            right_side->Layout();
        }
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

    // Add the tree root

    wxTreeItemId root = tree->AddRoot("Sources");

    int num_sources = my_source_list.num_sources();
    for (int i=0; i<num_sources; i++)
        if (my_source_list.get_parent(i) < 0)
        {
            std::string source_name = my_source_list.get_name(i);
            wxTreeItemId r1   = tree->AppendItem(root, source_name, -1, -1, new ItemData(i));

            std::vector<int> source_children = my_source_list.get_children(i);
            for (unsigned int j=0; j<source_children.size(); j++)
            {
                std::string child_name = my_source_list.get_name(source_children[j]);
                tree->AppendItem(r1, child_name, -1, -1, new ItemData(source_children[j]));
            }

        }


    parent->SetSizer(sizer1);

}
