///
/// \file
///



#ifndef GDW_PANEL_LR2_H
#define GDW_PANEL_LR2_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "gdw_panel.h"


class gdw_panel_lr2 : public gdw_panel
{
public:
    gdw_panel_lr2 (wxWindow* parent);
    virtual ~gdw_panel_lr2();


private:
    void process_window_draw   ();
    void process_window_events (wxEvent* event);
    void process_execute       ();


    virtual void draw_left_side         (wxPanel *parent) = 0;
    virtual void draw_right_side_top    (wxPanel *parent) = 0;
    virtual void draw_right_side_bottom (wxPanel *parent) = 0;

    wxPanel*    right_side;
};

#endif

