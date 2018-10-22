///
/// \file
///



#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "gdw_TopFrame.h"
#include "id_manager.h"


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

    // Start allocating identifiers above those used by wxWidgets.

    id_manager::set_first_allowed_id (wxID_HIGHEST+1);

    // Now start up wxWidgets.

    TopFrame *frame = new TopFrame("GenDat Explorer", wxPoint(-1, -1), wxSize(1000, 500));
    frame->Show(true);
    return true;
}

/// OnExit is called after destroying all application windows and controls, but before wxWidgets cleanup.

int MyApp::OnExit()
{
    return 0;
}

