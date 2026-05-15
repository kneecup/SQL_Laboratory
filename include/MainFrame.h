#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include <memory>
#include "DatabaseManager.h"
#include "Person.h"

class MainFrame : public wxFrame {
private:
    std::unique_ptr<DatabaseManager> dbManager;
    
    // GUI элементы
    wxListView* listView;
    wxTextCtrl* txtFirstName;
    wxTextCtrl* txtLastName;
    wxTextCtrl* txtEmail;
    wxTextCtrl* txtAge;
    wxButton* btnCreate;
    wxButton* btnSave;
    wxButton* btnDelete;
    wxButton* btnRefresh;
    wxStaticText* lblStatus;
    
    int currentSelectedId;
    
    // Обработчики событий
    void OnCreateDatabase(wxCommandEvent& event);
    void OnOpenDatabase(wxCommandEvent& event);
    void OnAddPerson(wxCommandEvent& event);
    void OnUpdatePerson(wxCommandEvent& event);
    void OnDeletePerson(wxCommandEvent& event);
    void OnRefreshList(wxCommandEvent& event);
    void OnSelectPerson(wxListEvent& event);
    void OnClearForm(wxCommandEvent& event);
    
    void LoadDataToList();
    void ClearForm();
    void UpdateStatus(const wxString& message);
    void ShowError(const wxString& message);
    
public:
    MainFrame();
    virtual ~MainFrame();
    
    wxDECLARE_EVENT_TABLE();
};