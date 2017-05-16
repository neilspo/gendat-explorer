///
/// \class gdw_field_group gdw_form_input.h
///
/// \brief Create a group of labeled fields
///
/// This class handles ...
///


#include "gdw_field_group.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// \param[in] parent        pointer to the parent window object (usually a panel)
/// \param[in] vbox          pointer to the vertical box sizer that will layout the field
/// \param[in] id            default ID to use when creating the user input fields
/// \param[in] label_width   default label width
/// \param[in] field_width   default field width
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_field_group::gdw_field_group (wxWindow *parent, wxBoxSizer *vbox,
                                  int id, int label_width, int field_width)
{
    my_parent      = parent;
    my_vbox        = vbox;
    my_id          = id;
    my_label_width = label_width;
    my_field_width = field_width;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Create one user input field with a label.
///
/// \param[in] label         input field label
/// \param[in] default_value input field default value
///
/// \return    pointer to the wxTextCtrl that will contain the user input
///
////////////////////////////////////////////////////////////////////////////////////////////////////

wxTextCtrl* gdw_field_group::add_field(const std::string label, const std::string default_value)
{
    // Put the label and user input field beside each other.

    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

    // Create the label.

    wxStaticText* my_label = new wxStaticText(my_parent, wxID_ANY, label, wxDefaultPosition,
                                              wxSize(my_label_width, -1));
    hbox->Add(my_label, 0);

    // Create the user input field.


    wxTextCtrl* my_data = new wxTextCtrl(my_parent, my_id, default_value, wxDefaultPosition,
                                         wxSize(my_field_width, -1));
    hbox->Add(my_data, 1);

    // Add everything to the vertical box sizer.

    my_vbox->Add(hbox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);

    // Return a pointer to the wxTextCtrl that will contain the user input.

    return my_data;
}

