#pragma once
#include "DatabaseEntity.h"
#include <string>
#include <memory>
#include <vector>
#include <ostream>

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
    
    // Реализация виртуальных методов базового класса
    std::string getTableName() const override;
    std::vector<std::string> getFieldNames() const override;
    std::vector<std::string> getFieldValues() const override;
    void setFieldValue(const std::string& fieldName, const std::string& value) override;
    std::string toString() const override;
    
    // Геттеры и сеттеры с валидацией
    std::string getFirstName() const { return firstName; }
    void setFirstName(const std::string& name);
    
    std::string getLastName() const { return lastName; }
    void setLastName(const std::string& name);
    
    std::string getEmail() const { return email; }
    void setEmail(const std::string& email);
    
    int getAge() const { return age; }
    void setAge(int age);
    
    // Перегрузка операторов (дополнительный оператор, кроме трех в базовом классе)
    bool operator==(const Person& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Person& person);
    
    // Фабричный метод (возвращает умный указатель)
    static std::unique_ptr<Person> create(const std::string& firstName, 
                                          const std::string& lastName,
                                          const std::string& email, 
                                          int age);
};