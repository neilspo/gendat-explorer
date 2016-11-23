///
/// \file
///



#ifndef GDW_EDIT_H
#define GDW_EDIT_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "gdw_panel.h"

//class database;

class gdw_edit : public gdw_panel
{
public:
  gdw_edit (wxWindow* parent);
  //  gdw_edit (wxWindow* parent, database& db);
  ~gdw_edit();

private:
  // wxWindow* my_parent;
  // database* my_db;
};

#endif
