#ifndef GD_EDIT_H
#define GD_EDIT_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class database;

class gd_edit : public wxPanel
{
public:
	gd_edit (wxPanel* parent, database& db);
	~gd_edit();

private:
	wxPanel*  my_parent;
	database* my_db;
};

#endif