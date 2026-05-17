#include "MainFrame.h"
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <sstream>

MainFrame::MainFrame() 
    : wxFrame(nullptr, wxID_ANY, "Database Manager", 
              wxDefaultPosition, wxSize(900, 600)) 
{
    // Используем умный указатель для DatabaseManager
    dbManager = std::make_unique<DatabaseManager>();
    currentSelectedId = -1;
    
    // ============ Создание GUI элементов ============
    // Примечание: wxWidgets автоматически управляет памятью дочерних окон,
    // поэтому здесь допустимо использование raw-указателей (требование про third-party).
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
    
    // --- Список записей ---
    listView = new wxListView(panel, wxID_ANY, wxDefaultPosition, wxSize(400, 300));
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
    
    // Размещение элементов формы в GridBagSizer
    formSizer->Add(lblFirstName, wxGBPosition(0, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtFirstName, wxGBPosition(0, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblLastName,  wxGBPosition(1, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtLastName,  wxGBPosition(1, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblEmail,     wxGBPosition(2, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtEmail,     wxGBPosition(2, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    formSizer->Add(lblAge,       wxGBPosition(3, 0), wxDefaultSpan, wxALL | wxALIGN_RIGHT, 5);
    formSizer->Add(txtAge,       wxGBPosition(3, 1), wxDefaultSpan, wxALL | wxEXPAND, 5);
    
    // Панель с кнопками действий
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
    mainSizer->Add(listView,  1, wxEXPAND | wxALL, 5);
    mainSizer->Add(formPanel, 0, wxEXPAND | wxALL, 5);
    
    panel->SetSizer(mainSizer);
    
    // ============ Привязка обработчиков событий через Bind() ============
    btnCreateDB->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) { OnCreateDatabase(evt); });
    btnOpenDB->Bind(wxEVT_BUTTON,   [this](wxCommandEvent& evt) { OnOpenDatabase(evt); });
    btnCreate->Bind(wxEVT_BUTTON,   [this](wxCommandEvent& evt) { OnAddPerson(evt); });
    btnSave->Bind(wxEVT_BUTTON,     [this](wxCommandEvent& evt) { OnUpdatePerson(evt); });
    btnDelete->Bind(wxEVT_BUTTON,   [this](wxCommandEvent& evt) { OnDeletePerson(evt); });
    btnRefresh->Bind(wxEVT_BUTTON,  [this](wxCommandEvent& evt) { OnRefreshList(evt); });
    btnClear->Bind(wxEVT_BUTTON,    [this](wxCommandEvent& evt) { OnClearForm(evt); });
    listView->Bind(wxEVT_LIST_ITEM_SELECTED, [this](wxListEvent& evt) { OnSelectPerson(evt); });
    
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
            
            // Создаем таблицу на основе структуры Person (полиморфизм: передаем по ссылке на базовый класс)
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
        
        // Фабричный метод возвращает умный указатель
        auto person = Person::create(firstName, lastName, email, age);
        
        // Демонстрация UPCAST и DOWNCAST:
        DatabaseEntity* basePtr = person.get();     // upcast: Person* -> DatabaseEntity*
        Person* personPtr = dynamic_cast<Person*>(basePtr);  // downcast: DatabaseEntity* -> Person*
        
        if (personPtr) {
            std::string personStr = personPtr->toString();  // используем downcast-указатель
            dbManager->insertEntity(std::move(person));     // перемещаем unique_ptr (upcast при передаче)
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
        
        // Демонстрация перегруженного оператора []
        std::string firstNameFromOperator = (*person)[0];
        
        dbManager->updateEntity(*person);  // передача по ссылке на базовый класс (полиморфизм)
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

void MainFrame::OnSelectPerson(wxListEvent& event) {
    long selectedIndex = event.GetIndex();
    if (selectedIndex < 0) return;
    
    // Получаем ID из первого столбца списка
    wxString idStr = listView->GetItemText(selectedIndex, 0);
    currentSelectedId = wxAtoi(idStr);
    
    try {
        // Получаем сущность по ID (возвращается unique_ptr<DatabaseEntity>)
        auto entity = dbManager->getEntityById(currentSelectedId, "persons");
        if (entity) {
            // DOWNCAST: DatabaseEntity* -> Person*
            Person* person = dynamic_cast<Person*>(entity.get());
            if (person) {
                txtFirstName->SetValue(person->getFirstName());
                txtLastName->SetValue(person->getLastName());
                txtEmail->SetValue(person->getEmail());
                txtAge->SetValue(std::to_string(person->getAge()));
                
                // Демонстрация перегруженного оператора ==
                Person testPerson;
                if (*person == testPerson) {
                    // Не должно случиться для реальной записи, просто демонстрация
                }
                
                // Демонстрация перегруженного оператора <<
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
        // Получаем все сущности (полиморфизм: возвращается vector<unique_ptr<DatabaseEntity>>)
        auto entities = dbManager->getAllEntities("persons");
        
        // Проходим по базовым указателям и делаем downcast
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