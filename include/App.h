#pragma once
#include <wx/wx.h>

class DatabaseApp : public wxApp {
public:
    virtual bool OnInit() override;
};

wxDECLARE_APP(DatabaseApp);