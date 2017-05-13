#include "gdw_form_input.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Create one user input field with a label.
///
/// \param[in] parent        pointer to the parent window object (usually a panel)
/// \param[in] vbox          pointer to the vertical box sizer that will layout the field
/// \param[in] label         input field label
/// \param[in] default_value input field default value
///
/// \return    pointer to the wxTextCtrl that will contain the user input
///
////////////////////////////////////////////////////////////////////////////////////////////////////


wxTextCtrl* gdw_form_input (wxWindow *parent, wxBoxSizer *vbox,
                            const std::string label, const std::string default_value)
{

    // Put the label and user input field beside each other.

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

    // Create the label.

    wxStaticText* my_label = new wxStaticText(parent, wxID_ANY, label, wxDefaultPosition, wxSize(100, -1));
    hbox->Add(my_label, 0);

    // Create the user input field.


    wxTextCtrl* my_data = new wxTextCtrl(parent, wxID_ANY, default_value);
    hbox->Add(my_data, 1);

    // Add everything to the vertical box sizer.

    vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    // Return a pointer to the wxTextCtrl that will contain the user input.

    return my_data;
}
