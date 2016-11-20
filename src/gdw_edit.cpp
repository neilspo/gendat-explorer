///
/// \class gdw_edit gdw_edit.h
///
/// \brief Provides the GenDat edit function.
///
/// This class ...
///



#include "gdw_edit.h"

/*

Constructor

*/

gdw_edit::gdw_edit(wxWindow* parent) : gdw_panel(parent)
{
  std::cout << "gdw_edit Constructor: Start" << std::endl;


  std::cout << "gdw_edit Constructor: End" << std::endl;

	// my_parent = parent;
	// my_db     = &db;
	
	// // Clear the parent window.

	// my_parent->DestroyChildren();


	// wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	// sizer->Add(new wxStaticText(parent, wxID_ANY, wxT("Test Window")), 0, wxALL, 5);
}

gdw_edit::~gdw_edit()
{
  std::cout << "gdw_edit Destructor: Start" << std::endl;
  
  //	my_parent->DestroyChildren();
}
