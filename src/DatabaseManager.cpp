// ============ Получение сущностей с сортировкой ============

std::vector<std::unique_ptr<DatabaseEntity>> DatabaseManager::getAllEntitiesSorted(
    const std::string& tableName, 
    const std::string& sortField, 
    bool ascending) 
{
    std::vector<std::unique_ptr<DatabaseEntity>> entities;
    
    // Защита от SQL-инъекций: проверяем, что sortField — одно из разрешённых полей
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
            
            // UPCAST: unique_ptr<Person> -> unique_ptr<DatabaseEntity>
            entities.push_back(std::move(person));
        }
    }
    
    return entities;
}