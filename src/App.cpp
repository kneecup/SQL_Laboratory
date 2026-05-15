#include "App.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(DatabaseApp);

bool DatabaseApp::OnInit() {
    auto* frame = new MainFrame();
    frame->Show(true);
    return true;
}