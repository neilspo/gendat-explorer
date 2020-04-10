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
#include <wx/event.h>

#include "database.h"
#include "gdw_panel_lr2.h"
#include "id_manager.h"

class gdw_db_ops : public gdw_panel_lr2
{
public:
    gdw_db_ops (wxWindow* parent, database* db);
    ~gdw_db_ops();


private:

    void draw_left_side         (wxPanel *parent);
    void draw_right_side_top    (wxPanel *parent);
    void draw_right_side_bottom (wxPanel *parent);
    bool has_unsaved_data       ();

    int left_side_wx_event_type ();

    wxPanel*    right_side;

    database*    my_db;
    wxListBox*   list_box;
    bool         unsaved_data_flag;

    id_manager   id_mgr;
    unsigned int list_box_id;

};

#endif

