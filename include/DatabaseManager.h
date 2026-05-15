#pragma once
#include <sqlite3.h>
#include <memory>
#include <vector>
#include <string>
#include "DatabaseEntity.h"
#include "Person.h"

class DatabaseManager {
private:
    sqlite3* db;
    std::string dbPath;
    bool isOpen;
    
    // Вспомогательные методы
    void executeSQL(const std::string& sql);
    static int callback(void* data, int argc, char** argv, char** azColName);
    
public:
    DatabaseManager();
    ~DatabaseManager();
    
    // Основные операции
    void createDatabase(const std::string& path);
    void openDatabase(const std::string& path);
    void closeDatabase();
    bool isDatabaseOpen() const { return isOpen; }
    std::string getCurrentDatabase() const { return dbPath; }
    
    // CRUD операции
    void createTable(const DatabaseEntity& entity);
    void insertEntity(std::unique_ptr<DatabaseEntity> entity);
    void updateEntity(const DatabaseEntity& entity);
    void deleteEntity(int id, const std::string& tableName);
    std::vector<std::unique_ptr<DatabaseEntity>> getAllEntities(const std::string& tableName);
    std::unique_ptr<DatabaseEntity> getEntityById(int id, const std::string& tableName);
    
    // Выполнение запросов
    std::vector<std::vector<std::string>> executeQuery(const std::string& query);
};