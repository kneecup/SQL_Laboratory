#pragma once
#include <string>
#include <memory>
#include <vector>
#include <stdexcept>

// Базовый абстрактный класс для всех сущностей БД
class DatabaseEntity {
protected:
    int id;
    std::string createdAt;
    
public:
    DatabaseEntity() : id(-1), createdAt("") {}
    virtual ~DatabaseEntity() = default;
    
    // Чисто виртуальные методы (полиморфизм)
    virtual std::string getTableName() const = 0;
    virtual std::vector<std::string> getFieldNames() const = 0;
    virtual std::vector<std::string> getFieldValues() const = 0;
    virtual void setFieldValue(const std::string& fieldName, const std::string& value) = 0;
    
    // Общие методы
    int getId() const { return id; }
    void setId(int newId) { id = newId; }
    
    std::string getCreatedAt() const { return createdAt; }
    void setCreatedAt(const std::string& time) { createdAt = time; }
    
    // Перегрузка операторов
    virtual bool operator==(const DatabaseEntity& other) const;
    virtual std::string operator[](int index) const;
    virtual DatabaseEntity& operator=(const DatabaseEntity& other);
    
    // Виртуальный деструктор для полиморфизма
    virtual std::string toString() const = 0;
};

// Исключения
class DatabaseException : public std::runtime_error {
public:
    explicit DatabaseException(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationException : public std::runtime_error {
public:
    explicit ValidationException(const std::string& msg) : std::runtime_error(msg) {}
};