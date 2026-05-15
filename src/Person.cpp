#include "Person.h"
#include <sstream>
#include <regex>

Person::Person() : DatabaseEntity(), firstName(""), lastName(""), email(""), age(0) {}

Person::Person(const std::string& firstName, const std::string& lastName, 
               const std::string& email, int age) 
    : DatabaseEntity(), firstName(firstName), lastName(lastName), email(email), age(age) {
    
    // Валидация
    if (firstName.empty() || lastName.empty()) {
        throw ValidationException("First name and last name cannot be empty");
    }
    if (age < 0 || age > 150) {
        throw ValidationException("Age must be between 0 and 150");
    }
    std::regex emailRegex(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
    if (!std::regex_match(email, emailRegex)) {
        throw ValidationException("Invalid email format");
    }
}

Person::Person(const Person& other) : DatabaseEntity(other) {
    firstName = other.firstName;
    lastName = other.lastName;
    email = other.email;
    age = other.age;
}

std::string Person::getTableName() const {
    return "persons";
}

std::vector<std::string> Person::getFieldNames() const {
    return {"first_name", "last_name", "email", "age"};
}

std::vector<std::string> Person::getFieldValues() const {
    return {firstName, lastName, email, std::to_string(age)};
}

void Person::setFieldValue(const std::string& fieldName, const std::string& value) {
    if (fieldName == "first_name") setFirstName(value);
    else if (fieldName == "last_name") setLastName(value);
    else if (fieldName == "email") setEmail(value);
    else if (fieldName == "age") setAge(std::stoi(value));
    else throw DatabaseException("Unknown field: " + fieldName);
}

std::string Person::toString() const {
    std::stringstream ss;
    ss << "Person[id=" << id << ", firstName=" << firstName 
       << ", lastName=" << lastName << ", email=" << email 
       << ", age=" << age << ", createdAt=" << createdAt << "]";
    return ss.str();
}

void Person::setFirstName(const std::string& name) {
    if (name.empty()) throw ValidationException("First name cannot be empty");
    firstName = name;
}

void Person::setLastName(const std::string& name) {
    if (name.empty()) throw ValidationException("Last name cannot be empty");
    lastName = name;
}

void Person::setEmail(const std::string& email) {
    std::regex emailRegex(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
    if (!std::regex_match(email, emailRegex)) throw ValidationException("Invalid email");
    this->email = email;
}

void Person::setAge(int age) {
    if (age < 0 || age > 150) throw ValidationException("Invalid age");
    this->age = age;
}

bool Person::operator==(const Person& other) const {
    return DatabaseEntity::operator==(other) && 
           firstName == other.firstName && 
           lastName == other.lastName &&
           email == other.email && 
           age == other.age;
}

std::ostream& operator<<(std::ostream& os, const Person& person) {
    os << person.toString();
    return os;
}

std::unique_ptr<Person> Person::create(const std::string& firstName, 
                                       const std::string& lastName,
                                       const std::string& email, 
                                       int age) {
    return std::make_unique<Person>(firstName, lastName, email, age);
}