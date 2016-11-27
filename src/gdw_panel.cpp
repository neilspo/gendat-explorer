///
/// \class gdw_panel gdw_panel.h
///
/// \brief Provides ...
///
/// This class handles ...
/// 

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/treectrl.h>

#include <string>
#include "gdw_panel.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// The constructor ...
///
////////////////////////////////////////////////////////////////////////////////////////////////////



gdw_panel::gdw_panel(wxWindow *parent) : wxPanel(parent)
{
  std::cout << "gdw_panel Constructor: Start" << std::endl;

  // wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  // sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Startup Window")), 0, wxALL, 5);
  // this->SetSizer(sizer);


  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

  wxTreeCtrl *tree = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

  sizer->Add(new wxStaticText(this, wxID_ANY, wxT("Test Window")), 0, wxALL, 5);
  sizer->Add(tree, 1, wxEXPAND);

  // tree->SetWindowStyle(wxTR_HIDE_ROOT); //hides the real root node to have many roots

  wxTreeItemId root = tree->AddRoot("Sources");
  wxTreeItemId r1   = tree->AppendItem(root, "1871 Census");
  wxTreeItemId r2   = tree->AppendItem(root, "1881 Census");
  tree->AppendItem(r1, "Node1");
  tree->AppendItem(r1, "Node2");
  tree->AppendItem(r2, "Node3");

  this->SetSizer(sizer);

  std::cout << "gdw_panel Constructor End" << std::endl;
}


gdw_panel::~gdw_panel()
{
  std::cout << "gdw_panel Destructor Start" << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Event handler
///
/// This member function handles all of the wxWidgets events that are generated by TopFrame.
///
////////////////////////////////////////////////////////////////////////////////////////////////////


void gdw_panel::event_handler (wxCommandEvent& event)
{
  std::cout << "TopFrame::event_handler: Start" << std::endl;

  int event_id = event.GetId();

}