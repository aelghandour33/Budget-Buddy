#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <mysqlx/xdevapi.h>
#include <string>
#include <vector>
#include <map>

class DBManager {
public:
    DBManager();
    ~DBManager();

    bool insert(const std::string& table, const std::map<std::string, std::string>& data);
    std::vector<std::map<std::string, std::string>> query(const std::string& queryStr);
    bool update(const std::string& table, const std::string& whereClause, const std::map<std::string, std::string>& data);
    bool remove(const std::string& table, const std::string& whereClause);

    void startTransaction();
    void commit();
    void rollback();

private:
    mysqlx::Session* session;
    std::string databaseName;
    void connect();
};

#endif
