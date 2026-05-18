#include "DatabaseManager.h"
#include <sstream>
#include <ctime>
#include <vector>

// ============ Вспомогательная структура для callback-функции SQLite ============
struct CallbackData {
    std::vector<std::vector<std::string>>* rows;
    std::vector<std::string>* columns;
};

// ============ Реализация вспомогательных методов ============

void DatabaseManager::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string error = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        throw DatabaseException("SQL error: " + error + " | Query: " + sql);
    }
}

int DatabaseManager::callback(void* data, int argc, char** argv, char** azColName) {
    auto* cbData = static_cast<CallbackData*>(data);
    
    if (cbData->columns->empty()) {
        for (int i = 0; i < argc; i++) {
            cbData->columns->push_back(azColName[i] ? azColName[i] : "");
        }
    }
    
    std::vector<std::string> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? argv[i] : "NULL");
    }
    cbData->rows->push_back(row);
    return 0;
}

// ============ Конструктор и деструктор ============

DatabaseManager::DatabaseManager() : db(nullptr), isOpen(false) {}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

// ============ Основные операции с базой данных ============

void DatabaseManager::createDatabase(const std::string& path) {
    closeDatabase();
    dbPath = path;
    
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        throw DatabaseException("Failed to create database: " + error);
    }
    isOpen = true;
}

void DatabaseManager::openDatabase(const std::string& path) {
    closeDatabase();
    dbPath = path;
    
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::string error = sqlite3_errmsg(db);
        sqlite3_close(db);
        db = nullptr;
        throw DatabaseException("Failed to open database: " + error);
    }
    isOpen = true;
}

void DatabaseManager::closeDatabase() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    isOpen = false;
    dbPath.clear();
}

// ============ CRUD операции ============

void DatabaseManager::createTable(const DatabaseEntity& entity) {
    if (!isOpen) throw DatabaseException("No database opened");
    
    std::stringstream sql;
    sql << "CREATE TABLE IF NOT EXISTS " << entity.getTableName() << " ("
        << "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        << "created_at TEXT DEFAULT CURRENT_TIMESTAMP";
    
    for (const auto& field : entity.getFieldNames()) {
        sql << ", " << field << " TEXT";
    }
    sql << ");";
    
    executeSQL(sql.str());
}

void DatabaseManager::insertEntity(std::unique_ptr<DatabaseEntity> entity) {
    if (!isOpen) throw DatabaseException("No database opened");
    
    std::stringstream sql;
    sql << "INSERT INTO " << entity->getTableName() << " (";
    
    auto fieldNames = entity->getFieldNames();
    for (size_t i = 0; i < fieldNames.size(); i++) {
        if (i > 0) sql << ", ";
        sql << fieldNames[i];
    }
    sql << ") VALUES (";
    
    auto fieldValues = entity->getFieldValues();
    for (size_t i = 0; i < fieldValues.size(); i++) {
        if (i > 0) sql << ", ";
        std::string escaped = fieldValues[i];
        size_t pos = 0;
        while ((pos = escaped.find("'", pos)) != std::string::npos) {
            escaped.insert(pos, "'");
            pos += 2;
        }
        sql << "'" << escaped << "'";
    }
    sql << ");";
    
    executeSQL(sql.str());
}

void DatabaseManager::updateEntity(const DatabaseEntity& entity) {
    if (!isOpen) throw DatabaseException("No database opened");
    if (entity.getId() < 0) throw DatabaseException("Invalid entity ID for update");
    
    std::stringstream sql;
    sql << "UPDATE " << entity.getTableName() << " SET ";
    
    auto fieldNames = entity.getFieldNames();
    auto fieldValues = entity.getFieldValues();
    
    for (size_t i = 0; i < fieldNames.size(); i++) {
        if (i > 0) sql << ", ";
        std::string escaped = fieldValues[i];
        size_t pos = 0;
        while ((pos = escaped.find("'", pos)) != std::string::npos) {
            escaped.insert(pos, "'");
            pos += 2;
        }
        sql << fieldNames[i] << " = '" << escaped << "'";
    }
    sql << " WHERE id = " << entity.getId() << ";";
    
    executeSQL(sql.str());
}

void DatabaseManager::deleteEntity(int id, const std::string& tableName) {
    if (!isOpen) throw DatabaseException("No database opened");
    
    std::stringstream sql;
    sql << "DELETE FROM " << tableName << " WHERE id = " << id << ";";
    executeSQL(sql.str());
}

std::vector<std::unique_ptr<DatabaseEntity>> DatabaseManager::getAllEntities(const std::string& tableName) {
    std::vector<std::unique_ptr<DatabaseEntity>> entities;
    
    if (tableName == "persons") {
        auto rows = executeQuery("SELECT id, created_at, first_name, last_name, email, age FROM persons ORDER BY id;");
        
        for (const auto& row : rows) {
            if (row.size() >= 6) {
                auto person = std::make_unique<Person>();
                person->setId(std::stoi(row[0]));
                person->setCreatedAt(row[1]);
                person->setFirstName(row[2]);
                person->setLastName(row[3]);
                person->setEmail(row[4]);
                person->setAge(std::stoi(row[5]));
                
                entities.push_back(std::move(person));
            }
        }
    }
    
    return entities;
}

// ============ Получение сущностей с сортировкой ============

std::vector<std::unique_ptr<DatabaseEntity>> DatabaseManager::getAllEntitiesSorted(
    const std::string& tableName, 
    const std::string& sortField, 
    bool ascending) 
{
    std::vector<std::unique_ptr<DatabaseEntity>> entities;
    
    // Белый список разрешённых полей (защита от SQL-инъекций)
    static const std::vector<std::string> allowedFields = {
        "id", "first_name", "last_name", "email", "age", "created_at"
    };
    
    bool fieldAllowed = false;
    for (const auto& field : allowedFields) {
        if (field == sortField) {
            fieldAllowed = true;
            break;
        }
    }
    
    if (!fieldAllowed) {
        throw DatabaseException("Invalid sort field: " + sortField);
    }
    
    // Формируем запрос с ORDER BY
    std::stringstream sql;
    sql << "SELECT id, created_at, first_name, last_name, email, age FROM "
        << tableName 
        << " ORDER BY " << sortField 
        << (ascending ? " ASC" : " DESC")
        << ";";
    
    auto rows = executeQuery(sql.str());
    
    for (const auto& row : rows) {
        if (row.size() >= 6) {
            auto person = std::make_unique<Person>();
            person->setId(std::stoi(row[0]));
            person->setCreatedAt(row[1]);
            person->setFirstName(row[2]);
            person->setLastName(row[3]);
            person->setEmail(row[4]);
            person->setAge(std::stoi(row[5]));
            
            entities.push_back(std::move(person));
        }
    }
    
    return entities;
}

// ============ Получение одной сущности по ID ============

std::unique_ptr<DatabaseEntity> DatabaseManager::getEntityById(int id, const std::string& tableName) {
    std::stringstream sql;
    sql << "SELECT id, created_at, first_name, last_name, email, age FROM " 
        << tableName << " WHERE id = " << id << ";";
    
    auto rows = executeQuery(sql.str());
    
    if (!rows.empty() && rows[0].size() >= 6) {
        auto person = std::make_unique<Person>();
        person->setId(std::stoi(rows[0][0]));
        person->setCreatedAt(rows[0][1]);
        person->setFirstName(rows[0][2]);
        person->setLastName(rows[0][3]);
        person->setEmail(rows[0][4]);
        person->setAge(std::stoi(rows[0][5]));
        
        return person;
    }
    
    return nullptr;
}

// ============ Выполнение произвольного запроса ============

std::vector<std::vector<std::string>> DatabaseManager::executeQuery(const std::string& query) {
    if (!isOpen) throw DatabaseException("No database opened");
    
    CallbackData cbData;
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
    cbData.columns = &columns;
    cbData.rows = &rows;
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), callback, &cbData, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::string error = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        throw DatabaseException("Query error: " + error + " | Query: " + query);
    }
    
    return rows;
}