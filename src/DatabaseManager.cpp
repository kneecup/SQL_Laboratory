#include "DatabaseManager.h"
#include <iostream>
#include <sstream>

DatabaseManager::DatabaseManager() : db(nullptr), isOpen(false) {}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

void DatabaseManager::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string error(errMsg);
        sqlite3_free(errMsg);
        throw DatabaseException("SQL Error: " + error);
    }
}

int DatabaseManager::callback(void* data, int argc, char** argv, char** azColName) {
    auto* rows = static_cast<std::vector<std::vector<std::string>>*>(data);
    std::vector<std::string> row;
    for (int i = 0; i < argc; i++) {
        row.push_back(argv[i] ? argv[i] : "NULL");
    }
    rows->push_back(row);
    return 0;
}

void DatabaseManager::createDatabase(const std::string& path) {
    closeDatabase();
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        throw DatabaseException("Can't create database: " + std::string(sqlite3_errmsg(db)));
    }
    dbPath = path;
    isOpen = true;
}

void DatabaseManager::openDatabase(const std::string& path) {
    closeDatabase();
    int rc = sqlite3_open(path.c_str(), &db);
    if (rc) {
        throw DatabaseException("Can't open database: " + std::string(sqlite3_errmsg(db)));
    }
    dbPath = path;
    isOpen = true;
}

void DatabaseManager::closeDatabase() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    isOpen = false;
}

void DatabaseManager::createTable(const DatabaseEntity& entity) {
    if (!isOpen) throw DatabaseException("No database is open");
    
    std::stringstream sql;
    sql << "CREATE TABLE IF NOT EXISTS " << entity.getTableName() << " (";
    sql << "id INTEGER PRIMARY KEY AUTOINCREMENT, ";
    
    auto fieldNames = entity.getFieldNames();
    for (size_t i = 0; i < fieldNames.size(); i++) {
        sql << fieldNames[i] << " TEXT";
        if (i < fieldNames.size() - 1) sql << ", ";
    }
    sql << ", created_at TEXT DEFAULT CURRENT_TIMESTAMP)";
    
    executeSQL(sql.str());
}

void DatabaseManager::insertEntity(std::unique_ptr<DatabaseEntity> entity) {
    if (!isOpen) throw DatabaseException("No database is open");
    
    std::stringstream sql;
    sql << "INSERT INTO " << entity->getTableName() << " (";
    
    auto fieldNames = entity->getFieldNames();
    auto fieldValues = entity->getFieldValues();
    
    for (size_t i = 0; i < fieldNames.size(); i++) {
        sql << fieldNames[i];
        if (i < fieldNames.size() - 1) sql << ", ";
    }
    sql << ") VALUES (";
    
    for (size_t i = 0; i < fieldValues.size(); i++) {
        sql << "'" << fieldValues[i] << "'";
        if (i < fieldValues.size() - 1) sql << ", ";
    }
    sql << ")";
    
    executeSQL(sql.str());
}

void DatabaseManager::updateEntity(const DatabaseEntity& entity) {
    if (!isOpen) throw DatabaseException("No database is open");
    if (entity.getId() == -1) throw DatabaseException("Entity has no ID");
    
    std::stringstream sql;
    sql << "UPDATE " << entity.getTableName() << " SET ";
    
    auto fieldNames = entity.getFieldNames();
    auto fieldValues = entity.getFieldValues();
    
    for (size_t i = 0; i < fieldNames.size(); i++) {
        sql << fieldNames[i] << " = '" << fieldValues[i] << "'";
        if (i < fieldNames.size() - 1) sql << ", ";
    }
    sql << " WHERE id = " << entity.getId();
    
    executeSQL(sql.str());
}

void DatabaseManager::deleteEntity(int id, const std::string& tableName) {
    if (!isOpen) throw DatabaseException("No database is open");
    
    std::stringstream sql;
    sql << "DELETE FROM " << tableName << " WHERE id = " << id;
    executeSQL(sql.str());
}

std::vector<std::unique_ptr<DatabaseEntity>> DatabaseManager::getAllEntities(const std::string& tableName) {
    if (!isOpen) throw DatabaseException("No database is open");
    
    std::stringstream sql;
    sql << "SELECT * FROM " << tableName;
    auto rows = executeQuery(sql.str());
    
    std::vector<std::unique_ptr<DatabaseEntity>> entities;
    
    // Upcast: создаем Person и приводим к DatabaseEntity через умный указатель
    for (const auto& row : rows) {
        auto person = std::make_unique<Person>();
        if (row.size() > 0) person->setId(std::stoi(row[0]));
        if (row.size() > 1) person->setFirstName(row[1]);
        if (row.size() > 2) person->setLastName(row[2]);
        if (row.size() > 3) person->setEmail(row[3]);
        if (row.size() > 4) person->setAge(std::stoi(row[4]));
        if (row.size() > 5) person->setCreatedAt(row[5]);
        
        // Upcast: Person* к DatabaseEntity* (автоматически через unique_ptr)
        entities.push_back(std::move(person));
    }
    
    return entities;
}

std::unique_ptr<DatabaseEntity> DatabaseManager::getEntityById(int id, const std::string& tableName) {
    if (!isOpen) throw DatabaseException("No database is open");
    
    std::stringstream sql;
    sql << "SELECT * FROM " << tableName << " WHERE id = " << id;
    auto rows = executeQuery(sql.str());
    
    if (rows.empty()) return nullptr;
    
    auto person = std::make_unique<Person>();
    const auto& row = rows[0];
    if (row.size() > 0) person->setId(std::stoi(row[0]));
    if (row.size() > 1) person->setFirstName(row[1]);
    if (row.size() > 2) person->setLastName(row[2]);
    if (row.size() > 3) person->setEmail(row[3]);
    if (row.size() > 4) person->setAge(std::stoi(row[4]));
    if (row.size() > 5) person->setCreatedAt(row[5]);
    
    return person;
}

std::vector<std::vector<std::string>> DatabaseManager::executeQuery(const std::string& query) {
    if (!isOpen) throw DatabaseException("No database is open");
    
    std::vector<std::vector<std::string>> rows;
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), callback, &rows, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::string error(errMsg);
        sqlite3_free(errMsg);
        throw DatabaseException("Query error: " + error);
    }
    
    return rows;
}