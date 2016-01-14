///
/// \class gd_edit gd_edit.h
///
/// \brief Provides the GenDat edit function.
///
/// This class ...
///



#include "gd_edit.h"

/*

Constructor

*/

gd_edit::gd_edit(wxPanel* parent, database& db)
{
	my_parent = parent;
	my_db     = &db;
	
	// Clear the parent window.

	my_parent->DestroyChildren();


	wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(new wxStaticText(parent, wxID_ANY, wxT("Test Window")), 0, wxALL, 5);
}

gd_edit::~gd_edit()
{
	my_parent->DestroyChildren();
}