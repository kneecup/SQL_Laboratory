#include "DatabaseEntity.h"
#include <sstream>

bool DatabaseEntity::operator==(const DatabaseEntity& other) const {
    return id == other.id;
}

std::string DatabaseEntity::operator[](int index) const {
    auto values = getFieldValues();
    if (index >= 0 && index < static_cast<int>(values.size())) {
        return values[index];
    }
    throw DatabaseException("Index out of range");
}

DatabaseEntity& DatabaseEntity::operator=(const DatabaseEntity& other) {
    if (this != &other) {
        id = other.id;
        createdAt = other.createdAt;
    }
    return *this;
}