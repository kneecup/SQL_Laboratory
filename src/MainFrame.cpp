#include "MainFrame.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <sstream>

MainFrame::MainFrame() 
    : wxFrame(nullptr, wxID_ANY, "Database Manager", 
              wxDefaultPosition, wxSize(900, 650)) 
{
    dbManager = std::make_unique<DatabaseManager>();
    currentSelectedId = -1;
    
    // ============ Создание GUI элементов ============
    auto* panel = new wxPanel(this);
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // --- Верхняя панель с кнопками управления БД ---
    auto* dbPanel = new wxPanel(panel);
    auto* dbSizer = new wxBoxSizer(wxHORIZONTAL);
    
    auto* btnCreateDB = new wxButton(dbPanel, wxID_ANY, "Create Database");
    auto* btnOpenDB = new wxButton(dbPanel, wxID_ANY, "Open Database");
    lblStatus = new wxStaticText(dbPanel, wxID_ANY, "Database: Not opened");
    
    dbSizer->Add(btnCreateDB, 0, wxALL, 5);
    dbSizer->Add(btnOpenDB, 0, wxALL, 5);
    dbSizer->Add(lblStatus, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    dbPanel->SetSizer(dbSizer);
    
    // --- Панель сортировки (НОВАЯ) ---
    auto* sortPanel = new wxPanel(panel);
    auto* sortSizer = new wxBoxSizer(wxHORIZONTAL);
    
    auto* lblSort = new wxStaticText(sortPanel, wxID_ANY, "Sort by:");
    cmbSortField = new wxComboBox(sortPanel, wxID_ANY, "id", 
                                   wxDefaultPosition, wxSize(120, -1));
    cmbSortField->Append("id");
    cmbSortField->Append("first_name");
    cmbSortField->Append("last_name");
    cmbSortField->Append("email");
    cmbSortField->Append("age");
    cmbSortField->SetSelection(0);  // выбрано "id" по умолчанию
    
    btnSortAsc  = new wxButton(sortPanel, wxID_ANY, L"\u25B2 Asc");   // ▲
    btnSortDesc = new wxButton(sortPanel, wxID_ANY, L"\u25BC Desc");  // ▼
    
    sortSizer->Add(lblSort, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    sortSizer->Add(cmbSortField, 0, wxALL, 5);
    sortSizer->Add(btnSortAsc, 0, wxALL, 5);
    sortSizer->Add(btnSortDesc, 0, wxALL, 5);
    sortPanel->SetSizer(sortSizer);
    
    // --- Список записей ---
    listView = new wxListView(panel, wxID_ANY, wxDefaultPosition, wxSize(400, 280));
    listView->AppendColumn("ID", wxLIST_FORMAT_LEFT, 50);
    listView->AppendColumn("First Name", wxLIST_FORMAT_LEFT, 120);
    listView->AppendColumn("Last Name", wxLIST_FORMAT_LEFT, 120);
    listView->AppendColumn("Email", wxLIST_FORMAT_LEFT, 200);
    listView->AppendColumn("Age", wxLIST_FORMAT_LEFT, 60);
    
    // --- Форма ввода/редактирования ---
    auto* formPanel = new wxPanel(panel);
    auto* formSizer = new wxGridBagSizer(5, 5);
    
    auto* lblFirstName = new wxStaticText(formPanel, wxID_ANY, "First Name:");
    auto* lblLastName  = new wxStaticText(formPanel, wxID_ANY, "Last Name:");
    auto* lblEmail     = new wxStaticText(formPanel, wxID_ANY, "Email:");
    auto* lblAge       = new wxStaticText(formPanel, wxID_ANY, "Age:");
    
    txtFirstName = new wxTextCtrl(formPanel, wxID_ANY);
    txtLastName  = new wxTextCtrl(formPanel, wxID_ANY);
    txtEmail     = new wxTextCtrl(formPanel, wxID_ANY);
    txtAge       = new wxTextCtrl(formPanel, wxID_ANY);
    
    btnCreate  = new wxButton(formPanel, wxID_ANY, "Add Person");
    btnSave    = new wxButton(formPanel, wxID_ANY, "Update Person");
    btnDelete  = new wxButton(formPanel, wxID_ANY, "Delete Person");
    btnRefresh = new wxButton(formPanel, wxID_ANY, "Refresh");
    auto* btnClear = new wxButton(formPanel, wxID_ANY, "Clear Form");
    
    formSizer->Add(lblFirstName, wxGBPosition(0, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtFirstName, wxGBPosition(0, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblLastName,  wxGBPosition(1, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtLastName,  wxGBPosition(1, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblEmail,     wxGBPosition(2, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtEmail,     wxGBPosition(2, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblAge,       wxGBPosition(3, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtAge,       wxGBPosition(3, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    
    auto* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(btnCreate,  0, wxALL, 5);
    btnSizer->Add(btnSave,    0, wxALL, 5);
    btnSizer->Add(btnDelete,  0, wxALL, 5);
    btnSizer->Add(btnRefresh, 0, wxALL, 5);
    btnSizer->Add(btnClear,   0, wxALL, 5);
    
    formSizer->Add(btnSizer, wxGBPosition(4, 0), wxGBSpan(1, 2), wxALL | wxALIGN_CENTER, 5);
    formSizer->AddGrowableCol(1);
    formPanel->SetSizer(formSizer);
    
    // --- Сборка главного окна ---
    mainSizer->Add(dbPanel,   0, wxEXPAND | wxALL, 5);
    mainSizer->Add(sortPanel, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(listView,  1, wxEXPAND | wxALL, 5);
    mainSizer->Add(formPanel, 0, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    
    // ============ Привязка обработчиков событий ============
    btnCreateDB->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) { OnCreateDatabase(evt); });
    btnOpenDB->Bind(wxEVT_BUTTON,   [this](wxCommandEvent& evt) { OnOpenDatabase(evt); });
    btnCreate->Bind(wxEVT_BUTTON,   [this](wxCommandEvent& evt) { OnAddPerson(evt); });
    btnSave->Bind(wxEVT_BUTTON,     [this](wxCommandEvent& evt) { OnUpdatePerson(evt); });
    btnDelete->Bind(wxEVT_BUTTON,   [this](wxCommandEvent& evt) { OnDeletePerson(evt); });
    btnRefresh->Bind(wxEVT_BUTTON,  [this](wxCommandEvent& evt) { OnRefreshList(evt); });
    btnClear->Bind(wxEVT_BUTTON,    [this](wxCommandEvent& evt) { OnClearForm(evt); });
    listView->Bind(wxEVT_LIST_ITEM_SELECTED, [this](wxListEvent& evt) { OnSelectPerson(evt); });
    
    // Привязка сортировки (НОВЫЕ)
    btnSortAsc->Bind(wxEVT_BUTTON,  [this](wxCommandEvent& evt) { OnSortAscending(evt); });
    btnSortDesc->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) { OnSortDescending(evt); });
    
    UpdateStatus("Ready. Please create or open a database.");
}

MainFrame::~MainFrame() = default;

// ============ Обработчики событий ============

void MainFrame::OnCreateDatabase(wxCommandEvent& event) {
    wxFileDialog dialog(this, "Save Database As", "", "", 
                        "SQLite Database (*.db)|*.db", 
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() == wxID_OK) {
        try {
            std::string path = dialog.GetPath().ToStdString();
            dbManager->createDatabase(path);
            
            Person tempPerson;
            dbManager->createTable(tempPerson);
            
            UpdateStatus("Database created: " + dialog.GetPath());
            LoadDataToList();
        } catch (const DatabaseException& e) {
            ShowError(e.what());
        }
    }
}

void MainFrame::OnOpenDatabase(wxCommandEvent& event) {
    wxFileDialog dialog(this, "Open Database", "", "", 
                        "SQLite Database (*.db)|*.db", 
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() == wxID_OK) {
        try {
            std::string path = dialog.GetPath().ToStdString();
            dbManager->openDatabase(path);
            UpdateStatus("Database opened: " + dialog.GetPath());
            LoadDataToList();
        } catch (const DatabaseException& e) {
            ShowError(e.what());
        }
    }
}

void MainFrame::OnAddPerson(wxCommandEvent& event) {
    if (!dbManager->isDatabaseOpen()) {
        ShowError("Please open or create a database first");
        return;
    }
    
    try {
        std::string firstName = txtFirstName->GetValue().ToStdString();
        std::string lastName  = txtLastName->GetValue().ToStdString();
        std::string email     = txtEmail->GetValue().ToStdString();
        int age               = std::stoi(txtAge->GetValue().ToStdString());
        
        auto person = Person::create(firstName, lastName, email, age);
        
        // UPCAST и DOWNCAST
        DatabaseEntity* basePtr = person.get();
        Person* personPtr = dynamic_cast<Person*>(basePtr);
        
        if (personPtr) {
            std::string personStr = personPtr->toString();
            dbManager->insertEntity(std::move(person));
            UpdateStatus("Person added successfully: " + personStr);
            LoadDataToList();
            ClearForm();
        }
    } catch (const ValidationException& e) {
        ShowError(e.what());
    } catch (const std::exception& e) {
        ShowError("Error: " + std::string(e.what()));
    }
}

void MainFrame::OnUpdatePerson(wxCommandEvent& event) {
    if (!dbManager->isDatabaseOpen()) {
        ShowError("Please open or create a database first");
        return;
    }
    
    if (currentSelectedId == -1) {
        ShowError("Please select a person to update");
        return;
    }
    
    try {
        auto person = std::make_unique<Person>();
        person->setId(currentSelectedId);
        person->setFirstName(txtFirstName->GetValue().ToStdString());
        person->setLastName(txtLastName->GetValue().ToStdString());
        person->setEmail(txtEmail->GetValue().ToStdString());
        person->setAge(std::stoi(txtAge->GetValue().ToStdString()));
        
        // Перегруженный оператор []
        std::string firstNameFromOperator = (*person)[0];
        
        dbManager->updateEntity(*person);
        UpdateStatus("Person updated successfully (first name from operator[]: " + firstNameFromOperator + ")");
        LoadDataToList();
        ClearForm();
    } catch (const ValidationException& e) {
        ShowError(e.what());
    } catch (const std::exception& e) {
        ShowError("Error: " + std::string(e.what()));
    }
}

void MainFrame::OnDeletePerson(wxCommandEvent& event) {
    if (!dbManager->isDatabaseOpen()) {
        ShowError("Please open or create a database first");
        return;
    }
    
    if (currentSelectedId == -1) {
        ShowError("Please select a person to delete");
        return;
    }
    
    if (wxMessageBox("Are you sure you want to delete this person?", "Confirm", 
                     wxYES_NO | wxICON_QUESTION, this) == wxYES) {
        try {
            dbManager->deleteEntity(currentSelectedId, "persons");
            UpdateStatus("Person deleted successfully");
            LoadDataToList();
            ClearForm();
        } catch (const DatabaseException& e) {
            ShowError(e.what());
        }
    }
}

void MainFrame::OnRefreshList(wxCommandEvent& event) {
    LoadDataToList();
}

// ============ НОВЫЕ ОБРАБОТЧИКИ СОРТИРОВКИ ============

void MainFrame::OnSortAscending(wxCommandEvent& event) {
    if (!dbManager->isDatabaseOpen()) {
        ShowError("Please open or create a database first");
        return;
    }
    
    std::string sortField = cmbSortField->GetValue().ToStdString();
    LoadSortedData(sortField, true);
    UpdateStatus("Sorted by " + sortField + " (ascending)");
}

void MainFrame::OnSortDescending(wxCommandEvent& event) {
    if (!dbManager->isDatabaseOpen()) {
        ShowError("Please open or create a database first");
        return;
    }
    
    std::string sortField = cmbSortField->GetValue().ToStdString();
    LoadSortedData(sortField, false);
    UpdateStatus("Sorted by " + sortField + " (descending)");
}

// ============ НОВЫЙ МЕТОД ЗАГРУЗКИ С СОРТИРОВКОЙ ============

void MainFrame::LoadSortedData(const std::string& sortField, bool ascending) {
    listView->DeleteAllItems();
    
    try {
        // Используем новый метод с сортировкой
        auto entities = dbManager->getAllEntitiesSorted("persons", sortField, ascending);
        
        for (const auto& entity : entities) {
            Person* person = dynamic_cast<Person*>(entity.get());
            if (person) {
                long index = listView->InsertItem(listView->GetItemCount(), 
                                                   std::to_string(person->getId()));
                listView->SetItem(index, 1, person->getFirstName());
                listView->SetItem(index, 2, person->getLastName());
                listView->SetItem(index, 3, person->getEmail());
                listView->SetItem(index, 4, std::to_string(person->getAge()));
            }
        }
        
        std::string direction = ascending ? "ASC" : "DESC";
        UpdateStatus("Loaded " + std::to_string(entities.size()) + 
                     " records (sorted by " + sortField + " " + direction + ")");
    } catch (const DatabaseException& e) {
        ShowError(e.what());
    }
}

// =========================================================

void MainFrame::OnSelectPerson(wxListEvent& event) {
    long selectedIndex = event.GetIndex();
    if (selectedIndex < 0) return;
    
    wxString idStr = listView->GetItemText(selectedIndex, 0);
    currentSelectedId = wxAtoi(idStr);
    
    try {
        auto entity = dbManager->getEntityById(currentSelectedId, "persons");
        if (entity) {
            Person* person = dynamic_cast<Person*>(entity.get());
            if (person) {
                txtFirstName->SetValue(person->getFirstName());
                txtLastName->SetValue(person->getLastName());
                txtEmail->SetValue(person->getEmail());
                txtAge->SetValue(std::to_string(person->getAge()));
                
                // Перегруженный оператор ==
                Person testPerson;
                if (*person == testPerson) {
                    // Демонстрация (не сработает для реальной записи)
                }
                
                // Перегруженный оператор <<
                std::stringstream ss;
                ss << *person;
                
                UpdateStatus("Selected: " + person->getFirstName() + " " + person->getLastName());
            }
        }
    } catch (const DatabaseException& e) {
        ShowError(e.what());
    }
}

void MainFrame::OnClearForm(wxCommandEvent& event) {
    ClearForm();
}

// ============ Вспомогательные методы ============

void MainFrame::LoadDataToList() {
    if (!dbManager->isDatabaseOpen()) return;
    
    listView->DeleteAllItems();
    
    try {
        auto entities = dbManager->getAllEntities("persons");
        
        for (const auto& entity : entities) {
            Person* person = dynamic_cast<Person*>(entity.get());
            if (person) {
                long index = listView->InsertItem(listView->GetItemCount(), 
                                                   std::to_string(person->getId()));
                listView->SetItem(index, 1, person->getFirstName());
                listView->SetItem(index, 2, person->getLastName());
                listView->SetItem(index, 3, person->getEmail());
                listView->SetItem(index, 4, std::to_string(person->getAge()));
            }
        }
        
        UpdateStatus("Loaded " + std::to_string(entities.size()) + " records");
    } catch (const DatabaseException& e) {
        ShowError(e.what());
    }
}

void MainFrame::ClearForm() {
    txtFirstName->Clear();
    txtLastName->Clear();
    txtEmail->Clear();
    txtAge->Clear();
    currentSelectedId = -1;
    UpdateStatus("Form cleared");
}

void MainFrame::UpdateStatus(const wxString& message) {
    lblStatus->SetLabel("Database: " + 
        (dbManager->isDatabaseOpen() ? dbManager->getCurrentDatabase() : "Not opened") + 
        " | " + message);
}

void MainFrame::ShowError(const wxString& message) {
    wxMessageBox(message, "Error", wxOK | wxICON_ERROR, this);
    UpdateStatus("Error: " + message);
}