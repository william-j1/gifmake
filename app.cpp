#include "app.h"
#include "mainframe.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainFrame* pMain = new MainFrame("GIFMake");
	pMain->Center();
	pMain->Show();
	return true;
}
