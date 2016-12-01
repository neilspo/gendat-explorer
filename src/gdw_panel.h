///
/// \file
///



#ifndef GDW_PANEL_H
#define GDW_PANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "id_manager.h"


class gdw_panel : public wxPanel
{
public:
        gdw_panel(wxWindow* parent);
        ~gdw_panel();
  

protected:
        unsigned int alloc_id ();
  
private:
        virtual void process_window_draw   () = 0;
        virtual void process_window_events (unsigned int event_id) {};

        void delayed_start ();
        void event_handler (wxCommandEvent& event);

        id_manager* id_mgr;
};

#endif
