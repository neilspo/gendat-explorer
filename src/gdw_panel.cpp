///
/// \class gdw_panel gdw_panel.h
///
/// \brief Base class for generating and managing GenDat display pages
///
/// This abstract base class provides a consistent interface for all of the GenDat display pages
/// and catches any exceptions thrown during execution of the derived classes.
/// Each of these derived classes handles one display page and must override the
/// following three pure virtual functions:
///
/// - process_window_draw
/// - process_execute
/// - process_window_events
///
/// If required, some derived classes may also need to override the following virtual function:
///
/// - has_unsaved_data
///

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "gdw_panel.h"



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Constructor
///
/// The constructor creates a wxPanel and requests that `process_window_draw()`, in the derived
/// class, be run after the derived constructor(s) have completed.
///
/// \param [in]   parent   pointer to the parent window
///
////////////////////////////////////////////////////////////////////////////////////////////////////

gdw_panel::gdw_panel(wxWindow *parent) : wxPanel(parent)
{
    CallAfter (&gdw_panel::delayed_start);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Run the derived class process_window_draw() method
///
/// This member function will run after the constructors of the base class and derived class
/// (or classes) have been completed.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel::delayed_start()
{
    try
    {
        process_window_draw();
    }
    catch (std::runtime_error& exception)
    {
        process_runtime_error (exception);
    }
    catch (std::logic_error& exception)
    {
        process_logic_error (exception);
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Reload page
///
/// This member function deletes the contents of the current GenDat display page and then
/// calls `process_window_draw()` to reload the page.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel::page_reload()
{
    if (ok_to_delete())
    {
        // Clear the main data display panel.

        DestroyChildren();

        // Have the derived class redraw the GenDat display page.

        try
        {
            process_window_draw();
        }
        catch (std::runtime_error& exception)
        {
            process_runtime_error (exception);
        }
        catch (std::logic_error& exception)
        {
            process_logic_error (exception);
        }
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Execute page task
///
/// This member function executes the main task of this GenDat display page and handles any
/// resulting C++ exceptions. Function `process_execute()` (which must be defined in any
/// derived class) is called to execute page specific code.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel::page_execute()
{
    try
    {
        process_execute();
    }
    catch (std::runtime_error& exception)
    {
        process_runtime_error (exception);
    }
    catch (std::logic_error& exception)
    {
        process_logic_error (exception);
    }
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Determine if page can be deleted
///
/// This member function first calls `has_unsaved_data()` to determine if deleting the current
/// GenDat display page would cause data loss. If so, then the function requests user confirmation
/// that page deletion should continue.
///
/// \return `true` if OK to the delete the page, `false` otherwise
///
////////////////////////////////////////////////////////////////////////////////////////////////////

bool gdw_panel::ok_to_delete()
{
    if (has_unsaved_data())
    {
        wxString message = "This page contains unsaved data.";
        wxMessageDialog* msg_dia = new wxMessageDialog(NULL, message, "Warning", wxYES|wxNO|wxCENTRE);
        if (msg_dia->ShowModal() == wxID_NO)
        {
            return false;
        }
    }
    return true;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Determine if page has unsaved data
///
/// This virtual member function determines if the page holds data that have not been saved in
/// the database. Deletion of the page would cause these data be lost.
///
/// \return   true if the page contains unsaved data, false otherwise
///
/// \note
/// This version of the function always returns false. Derived classes should override it
/// whenever user input could be lost.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

bool gdw_panel::has_unsaved_data()
{
    return false;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// \brief Event handler
///
/// This member function handles all of the wxWidgets events, and any resulting C++ exceptions,
/// that are generated by the GenDat display pages. Function `process_window_events` (which must be
/// defined in any derived class) is called to execute page specific code.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel::event_handler (wxEvent& event)
{
    try
    {
        process_window_events (&event);
    }
    catch (std::runtime_error& exception)
    {
        process_runtime_error (exception);
    }
    catch (std::logic_error& exception)
    {
        process_logic_error (exception);
    }

}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Process a runtime error. Report the error message in a popup window
// (a modal dialogue), but it should be safe to carry on.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel::process_runtime_error (std::runtime_error& exception)
{
    wxString error_msg = exception.what();
    wxMessageDialog* msg_dia = new wxMessageDialog(NULL, error_msg, "Runtime Error");
    msg_dia->ShowModal();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Process a program logic error (a bug). Report the error message in a popup window
// (a modal dialogue) and then terminate the program.
//
////////////////////////////////////////////////////////////////////////////////////////////////////

void gdw_panel::process_logic_error (std::logic_error& exception)
{
    // Generate a popup window (a modal dialogue) with the error message.

    wxString error_msg = exception.what();
    wxMessageDialog* msg_dia = new wxMessageDialog(NULL, error_msg, "Program Logic Error");
    msg_dia->ShowModal();

    // After the user closes the error window, safely close out the program.

    wxApp*    my_wxApp = wxTheApp;
    wxWindow* my_TopWindow = my_wxApp->GetTopWindow();
    my_TopWindow->Close();
}
