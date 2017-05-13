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

#endif
