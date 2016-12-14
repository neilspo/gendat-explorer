///
/// \file
///



#ifndef GDW_EDIT_H
#define GDW_EDIT_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/grid.h>

#include "database.h"
#include "db_row_set_w.h"
#include "gdw_panel.h"
#include "id_manager.h"

class gdw_edit : public gdw_panel
{
public:
        gdw_edit (wxWindow* parent, database* db);
        ~gdw_edit();


private:
        void process_window_draw ();
        bool has_unsaved_data() ;
        
        void process_window_events (wxEvent* event);

        database*    my_db;
        db_row_set_w row_set;
        wxGrid*      grid;
        bool         unsaved_data_flag;

        id_manager   id_mgr;
        unsigned int id_grid_event;
        unsigned int id_save_event;

};

#endif
