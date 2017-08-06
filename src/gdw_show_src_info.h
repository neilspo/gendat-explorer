///
/// \file
///



#ifndef GDW_SHOW_SRC_INFO_H
#define GDW_SHOW_SRC_INFO_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/grid.h>
#include <wx/treectrl.h>

#include "gde_source.h"
#include "gdw_panel.h"
#include "id_manager.h"

class gdw_show_src_info : public gdw_panel
{
public:
    gdw_show_src_info (wxWindow* parent, gendat_source_list source_list);
    ~gdw_show_src_info();


private:

    // Override the pure virtual functions in the base class.

    void process_window_draw   ();
    void process_execute       ();
    void process_window_events (wxEvent* event);

    // Private variables.

    gendat_source_list  my_source_list;
    id_manager          id_mgr;
    unsigned int        tree_id;
    wxTreeCtrl         *tree;

    void draw_left_panel (wxPanel *parent);

    // This class provides a way to attach a data value to the tree nodes.

    class ItemData : public wxTreeItemData
    {
    public:
        ItemData (int data) {my_data = data;};
        int GetData () {return my_data;};
    private:
        int my_data;
    };

};

#endif