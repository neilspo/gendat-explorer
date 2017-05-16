///
/// \file
///



#ifndef GDW_FORM_INPUT_H
#define GDW_FORM_INPUT_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>

wxTextCtrl* gdw_form_input (wxWindow *parent, wxBoxSizer *vbox,
                            const std::string label, const std::string default_value);

class gdw_field_group
{
public:
    gdw_field_group(wxWindow *parent, wxBoxSizer *vbox, int id, int label_width, int field_width);

    wxTextCtrl* add_field(const std::string label, const std::string default_value);
private:
    wxWindow*   my_parent;
    wxBoxSizer* my_vbox;
    int         my_id;
    int         my_label_width;
    int         my_field_width;
};


#endif
