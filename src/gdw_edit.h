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


class gdw_edit : public gdw_panel
{
public:
        gdw_edit (wxWindow* parent, database* db);
        ~gdw_edit();


private:
        void process_window_draw ();
        
        database*    my_db;
        db_row_set_w row_set;
        wxGrid*      grid;
};

#endif
