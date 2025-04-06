#include "main.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    if (!wxApp::OnInit())
        return false;

    MainFrame* frame = new MainFrame("Interactive GL Canvas with Controls");
    frame->Show(true);

    return true;
}
