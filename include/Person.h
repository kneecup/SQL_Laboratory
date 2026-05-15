#pragma once
#include "DatabaseEntity.h"
#include <string>
#include <memory>
#include <vector>

class Person : public DatabaseEntity {
private:
    std::string firstName;
    std::string lastName;
    std::string email;
    int age;
    
public:
    Person();
    Person(const std::string& firstName, const std::string& lastName, 
           const std::string& email, int age);
    Person(const Person& other);
    ~Person() override = default;
    
    // Реализация виртуальных методов
    std::string getTableName() const override;
    std::vector<std::string> getFieldNames() const override;
    std::vector<std::string> getFieldValues() const override;
    void setFieldValue(const std::string& fieldName, const std::string& value) override;
    std::string toString() const override;
    
    // Специфичные методы
    std::string getFirstName() const { return firstName; }
    void setFirstName(const std::string& name);
    
    std::string getLastName() const { return lastName; }
    void setLastName(const std::string& name);
    
    std::string getEmail() const { return email; }
    void setEmail(const std::string& email);
    
    int getAge() const { return age; }
    void setAge(int age);
    
    // Перегрузка операторов (продолжение)
    bool operator==(const Person& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Person& person);
    
    // Фабричный метод для создания умного указателя
    static std::unique_ptr<Person> create(const std::string& firstName, 
                                          const std::string& lastName,
                                          const std::string& email, 
                                          int age);
};