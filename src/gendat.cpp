///
/// \file
///



#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "gdw_TopFrame.h"


class MyApp : public wxApp
{
public:
  virtual bool OnInit();
  virtual int  OnExit();
};


///
/// OnInit() serves as the main procedure for wxWidgets programs.
///

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
  std::cout << "OnInit Start" << std::endl;
  TopFrame *frame = new TopFrame("GenDat Miner", wxPoint(-1, -1), wxSize(1000, 500));
  frame->Show(true);
  return true;
}

// OnExit is called after destroying all application windows and controls, but before wxWidgets cleanup.

int MyApp::OnExit()
{
  std::cout << "OnExit Start" << std::endl;
  return 0;
}

