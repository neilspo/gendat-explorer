///
/// \file
///



#ifndef GDW_SEARCH_H
#define GDW_SEARCH_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/grid.h>

#include "database.h"
#include "gdw_panel.h"
#include "id_manager.h"

class gdw_search : public gdw_panel
{
public:
    gdw_search (wxWindow* parent, database* db);
    ~gdw_search();


private:
    void process_window_draw ();
    void process_execute     ();
    bool has_unsaved_data    ();

    void process_window_events (wxEvent* event);

    database*    my_db;
    bool         unsaved_data_flag;
    id_manager   id_mgr;
    unsigned int id_text_event;
    wxTextCtrl*  wx_surname;
    wxTextCtrl*  wx_given_name;
};

#endif
