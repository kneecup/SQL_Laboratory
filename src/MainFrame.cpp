#include "MainFrame.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <sstream>

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(wxID_ANY, MainFrame::OnCreateDatabase)
    EVT_BUTTON(wxID_ANY, MainFrame::OnOpenDatabase)
    EVT_BUTTON(wxID_ANY, MainFrame::OnAddPerson)
    EVT_BUTTON(wxID_ANY, MainFrame::OnUpdatePerson)
    EVT_BUTTON(wxID_ANY, MainFrame::OnDeletePerson)
    EVT_BUTTON(wxID_ANY, MainFrame::OnRefreshList)
    EVT_BUTTON(wxID_ANY, MainFrame::OnClearForm)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, MainFrame::OnSelectPerson)
wxEND_EVENT_TABLE()

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Database Manager", wxDefaultPosition, wxSize(900, 600)) {
    dbManager = std::make_unique<DatabaseManager>();
    currentSelectedId = -1;
    
    // Создание панели
    auto* panel = new wxPanel(this);
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Верхняя панель с кнопками управления БД
    auto* dbPanel = new wxPanel(panel);
    auto* dbSizer = new wxBoxSizer(wxHORIZONTAL);
    
    auto* btnCreateDB = new wxButton(dbPanel, wxID_ANY, "Create Database");
    auto* btnOpenDB = new wxButton(dbPanel, wxID_ANY, "Open Database");
    auto* lblDBInfo = new wxStaticText(dbPanel, wxID_ANY, "Database: Not opened");
    lblStatus = lblDBInfo;
    
    dbSizer->Add(btnCreateDB, 0, wxALL, 5);
    dbSizer->Add(btnOpenDB, 0, wxALL, 5);
    dbSizer->Add(lblDBInfo, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    dbPanel->SetSizer(dbSizer);
    
    // Центральная панель со списком
    listView = new wxListView(panel, wxID_ANY, wxDefaultPosition, wxSize(400, 300));
    listView->AppendColumn("ID", wxLIST_FORMAT_LEFT, 50);
    listView->AppendColumn("First Name", wxLIST_FORMAT_LEFT, 120);
    listView->AppendColumn("Last Name", wxLIST_FORMAT_LEFT, 120);
    listView->AppendColumn("Email", wxLIST_FORMAT_LEFT, 200);
    listView->AppendColumn("Age", wxLIST_FORMAT_LEFT, 60);
    
    // Нижняя панель с формой ввода
    auto* formPanel = new wxPanel(panel);
    auto* formSizer = new wxGridBagSizer(5, 5);
    
    auto* lblFirstName = new wxStaticText(formPanel, wxID_ANY, "First Name:");
    auto* lblLastName = new wxStaticText(formPanel, wxID_ANY, "Last Name:");
    auto* lblEmail = new wxStaticText(formPanel, wxID_ANY, "Email:");
    auto* lblAge = new wxStaticText(formPanel, wxID_ANY, "Age:");
    
    txtFirstName = new wxTextCtrl(formPanel, wxID_ANY);
    txtLastName = new wxTextCtrl(formPanel, wxID_ANY);
    txtEmail = new wxTextCtrl(formPanel, wxID_ANY);
    txtAge = new wxTextCtrl(formPanel, wxID_ANY);
    
    btnCreate = new wxButton(formPanel, wxID_ANY, "Add Person");
    btnSave = new wxButton(formPanel, wxID_ANY, "Update Person");
    btnDelete = new wxButton(formPanel, wxID_ANY, "Delete Person");
    btnRefresh = new wxButton(formPanel, wxID_ANY, "Refresh");
    auto* btnClear = new wxButton(formPanel, wxID_ANY, "Clear Form");
    
    formSizer->Add(lblFirstName, wxGBPosition(0, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtFirstName, wxGBPosition(0, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblLastName, wxGBPosition(1, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtLastName, wxGBPosition(1, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblEmail, wxGBPosition(2, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtEmail, wxGBPosition(2, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblAge, wxGBPosition(3, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtAge, wxGBPosition(3, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    
    auto* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(btnCreate, 0, wxALL, 5);
    btnSizer->Add(btnSave, 0, wxALL, 5);
    btnSizer->Add(btnDelete, 0, wxALL, 5);
    btnSizer->Add(btnRefresh, 0, wxALL, 5);
    btnSizer->Add(btnClear, 0, wxALL, 5);
    
    formSizer->Add(btnSizer, wxGBPosition(4, 0), wxGBSpan(1, 2), wxALL | wxALIGN_CENTER, 5);
    formSizer->AddGrowableCol(1);
    formPanel->SetSizer(formSizer);
    
    // Сборка главного окна
    mainSizer->Add(dbPanel, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(listView, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(formPanel, 0, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    
    // Привязка обработчиков (downcast через лямбды)
    btnCreateDB->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnCreateDatabase(event); });
    btnOpenDB->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnOpenDatabase(event); });
    btnCreate->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnAddPerson(event); });
    btnSave->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnUpdatePerson(event); });
    btnDelete->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnDeletePerson(event); });
    btnRefresh->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { OnRefreshList(event); });
    btnClear->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { ClearForm(); });
    
    UpdateStatus("Ready. Please create or open a database.");
}

MainFrame::~MainFrame() = default;

void MainFrame::OnCreateDatabase(wxCommandEvent& event) {
    wxFileDialog dialog(this, "Save Database As", "", "", "SQLite Database (*.db)|*.db", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (dialog.ShowModal() == wxID_OK) {
        try {
            std::string path = dialog.GetPath().ToStdString();
            dbManager->createDatabase(path);
            
            // Создаем таблицу через Person
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
    wxFileDialog dialog(this, "Open Database", "", "", "SQLite Database (*.db)|*.db", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
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
        std::string lastName = txtLastName->GetValue().ToStdString();
        std::string email = txtEmail->GetValue().ToStdString();
        int age = std::stoi(txtAge->GetValue().ToStdString());
        
        // Использование фабричного метода с умным указателем
        auto person = Person::create(firstName, lastName, email, age);
        
        // Downcast: умный указатель на DatabaseEntity -> Person (через dynamic_cast)
        // Здесь мы точно знаем, что это Person, но покажем пример upcast/downcast
        DatabaseEntity* basePtr = person.get();  // Upcast
        Person* personPtr = dynamic_cast<Person*>(basePtr);  // Downcast
        if (personPtr) {
            dbManager->insertEntity(std::move(person));
            UpdateStatus("Person added successfully");
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
        
        // Демонстрация перегрузки оператора []
        std::string firstNameFromOperator = (*person)[0];
        
        dbManager->updateEntity(*person);
        UpdateStatus("Person updated successfully");
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

void MainFrame::OnSelectPerson(wxListEvent& event) {
    currentSelectedId = event.GetIndex() + 1;  // ID = индекс + 1
    
    try {
        auto entity = dbManager->getEntityById(currentSelectedId, "persons");
        if (entity) {
            // Downcast для доступа к специфичным методам Person
            Person* person = dynamic_cast<Person*>(entity.get());
            if (person) {
                txtFirstName->SetValue(person->getFirstName());
                txtLastName->SetValue(person->getLastName());
                txtEmail->SetValue(person->getEmail());
                txtAge->SetValue(std::to_string(person->getAge()));
                
                // Демонстрация перегрузки оператора ==
                Person testPerson;
                if (*person == testPerson) {
                    // Не равно, ничего не делаем
                }
                
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

void MainFrame::LoadDataToList() {
    if (!dbManager->isDatabaseOpen()) return;
    
    listView->DeleteAllItems();
    
    try {
        auto entities = dbManager->getAllEntities("persons");
        
        // Демонстрация полиморфизма: работа с базовым классом
        for (const auto& entity : entities) {
            // Upcast уже выполнен в getAllEntities
            Person* person = dynamic_cast<Person*>(entity.get());
            if (person) {
                long index = listView->InsertItem(listView->GetItemCount(), std::to_string(person->getId()));
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
    lblStatus->SetLabel("Database: " + (dbManager->isDatabaseOpen() ? 
                       dbManager->getCurrentDatabase() : "Not opened") + " | " + message);
}

void MainFrame::ShowError(const wxString& message) {
    wxMessageBox(message, "Error", wxOK | wxICON_ERROR, this);
    UpdateStatus("Error: " + message);
}