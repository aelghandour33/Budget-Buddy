#include "DBManager.h"
#include <iostream>
#include <fstream>
#include <cstdlib>

DBManager::DBManager() {
    connect();
}

DBManager::~DBManager() {
    if (session) delete session;
}

void DBManager::connect() {
    std::ifstream env("/Users/ahmedelghandour/Desktop/se3313_project/server/.env");
    std::string line, host, user, pass;

    auto stripQuotes = [](std::string val) {
        if (!val.empty() && val.front() == '"' && val.back() == '"') {
            return val.substr(1, val.length() - 2);
        }
        return val;
    };

    while (std::getline(env, line)) {
        std::cout << "Reading line: " << line << std::endl;

        if (line.find("MYSQL_HOST=") == 0) {
            host = stripQuotes(line.substr(11));
            std::cout << "Parsed HOST: " << host << std::endl;
        }
        else if (line.find("MYSQL_USER=") == 0) {
            user = stripQuotes(line.substr(11));
            std::cout << "Parsed USER: " << user << std::endl;
        }
        else if (line.find("MYSQL_PASSWORD=") == 0) {
            pass = stripQuotes(line.substr(15));
            std::cout << "Parsed PASS: " << pass << std::endl;
        }
        else if (line.find("MYSQL_DB=") == 0) {
            databaseName = stripQuotes(line.substr(9));
            std::cout << "Parsed DB: " << databaseName << std::endl;
        }
    }

    std::cout << "Connecting to DB at host=" << host << ", user=" << user << ", db=" << databaseName << std::endl;

    session = new mysqlx::Session(host, 33060, user, pass);
    session->sql("USE " + databaseName).execute();
}


bool DBManager::insert(const std::string& table, const std::map<std::string, std::string>& data) {
    auto schema = session->getSchema(databaseName);
    auto tbl = schema.getTable(table);
    std::vector<std::string> keys, values;
    for (const auto& kv : data) {
        keys.push_back(kv.first);
        values.push_back(kv.second);
    }

    try {
        tbl.insert(keys).values(values).execute();
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::map<std::string, std::string>> DBManager::query(const std::string& queryStr) {
    std::vector<std::map<std::string, std::string>> results;
    try {
        session->sql("USE " + databaseName).execute();

        auto sql = session->sql(queryStr).execute();
        while (sql.hasData()) {
            auto row = sql.fetchOne();

            if (!row) {
                continue;  // skip null row
            }

            std::map<std::string, std::string> rowData;
            for (int i = 0; i < row.colCount(); ++i) {
                std::string colName = sql.getColumn(i).getColumnLabel();
                std::string colValue;

                try {
                    if (!row[i].isNull()) {
                        colValue = row[i].get<std::string>();
                    } else {
                        colValue = "";
                    }
                } catch (...) {
                    try {
                        colValue = std::to_string(row[i].get<int>());
                    } catch (...) {
                        try {
                            colValue = std::to_string(row[i].get<double>());
                        } catch (...) {
                            colValue = "[unprintable]";
                        }
                    }
                }

                rowData[colName] = colValue;
            }

            results.push_back(rowData);
        }

        std::cout << "[DBManager] Final row count: " << results.size() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[DBManager] Exception: " << e.what() << std::endl;
    }

    return results;
}



bool DBManager::update(const std::string& table, const std::string& whereClause, const std::map<std::string, std::string>& data) {
    try {
        auto schema = session->getSchema(databaseName);
        std::string updateStr = "UPDATE " + table + " SET ";
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it != data.begin()) updateStr += ", ";
            updateStr += it->first + "='" + it->second + "'";
        }
        updateStr += " WHERE " + whereClause;
        session->sql(updateStr).execute();
        return true;
    } catch (...) {
        return false;
    }
}

bool DBManager::remove(const std::string& table, const std::string& whereClause) {
    try {
        auto schema = session->getSchema(databaseName);
        std::string deleteStr = "DELETE FROM " + table + " WHERE " + whereClause;
        session->sql(deleteStr).execute();
        return true;
    } catch (...) {
        return false;
    }
}

void DBManager::startTransaction() {
    session->startTransaction();
}

void DBManager::commit() {
    session->commit();
}

void DBManager::rollback() {
    session->rollback();
}
