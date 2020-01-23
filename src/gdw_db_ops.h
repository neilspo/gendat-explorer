///
/// \file
///



#ifndef GDW_DB_OPS_H
#define GDW_DB_OPS_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/listbox.h>

#include "database.h"
#include "gdw_panel.h"
#include "id_manager.h"

class gdw_db_ops : public gdw_panel
{
public:
    gdw_db_ops (wxWindow* parent, database* db);
    ~gdw_db_ops();


private:
    void process_window_draw   ();
    void process_window_events (wxEvent* event);
    void process_execute       ();
    bool has_unsaved_data      ();

    void draw_left_side (wxPanel *parent);

    wxPanel*    right_side;

    database*    my_db;
    wxListBox*   list_box;
    bool         unsaved_data_flag;

    id_manager   id_mgr;
    unsigned int list_box_id;

};

#endif

