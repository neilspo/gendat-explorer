#ifndef GENDAT_H
#define GENDAT_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <string>
#include "database.h"

void gd_edit (wxPanel* main_panel, database& gendat_db);
void gd_link (wxPanel* main_panel, database& gendat_db);


#endif
