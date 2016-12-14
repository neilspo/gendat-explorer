///
/// \file
///



#ifndef GDW_PANEL_H
#define GDW_PANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <stdexcept>


class gdw_panel : public wxPanel
{
public:
        gdw_panel(wxWindow* parent);
        ~gdw_panel();

        void page_reload();
        
        virtual bool page_save();
        virtual bool has_unsaved_data();
        virtual bool ok_to_delete();
        
protected:
        void event_handler (wxEvent& event);
  
private:
        virtual void process_window_draw() = 0;
        virtual void process_window_events (wxEvent* event) = 0;

        void delayed_start ();
        void process_runtime_error (std::runtime_error& exception);
        void process_logic_error   (std::logic_error&   exception);
};

#endif
