#include "ExpenseManager.h"
#include <sstream>

ExpenseManager::ExpenseManager(DBManager* db, TransactionManager* txn)
    : dbManager(db), txnManager(txn) {}

std::string ExpenseManager::addExpense(const std::string& username, const picojson::value& data) {
    auto rows = dbManager->query("SELECT id FROM users WHERE username = '" + username + "'");
    if (rows.empty()) return "{\"status\":\"error\",\"message\":\"User not found\"}";
    std::string userId = rows[0]["id"];

    txnManager->beginTransaction();
    bool success = dbManager->insert("expenses", {
        {"user_id", userId},
        {"amount", data.get("amount").to_str()},
        {"category", data.get("category").to_str()},
        {"note", data.get("note").to_str()},
        {"date", data.get("date").to_str()}
    });

    if (success) {
        txnManager->commitTransaction();
        return "{\"status\":\"success\"}";
    } else {
        txnManager->rollbackTransaction();
        return "{\"status\":\"error\",\"message\":\"Add failed\"}";
    }
}

std::string ExpenseManager::getExpenses(const std::string& username, const picojson::value& filter) {
    auto rows = dbManager->query("SELECT id FROM users WHERE username = '" + username + "'");
    if (rows.empty()) return "{\"status\":\"error\",\"message\":\"User not found\"}";
    std::string userId = rows[0]["id"];

    std::stringstream ss;
    ss << "SELECT * FROM expenses WHERE user_id='" << userId << "'";

    if (filter.contains("category")) {
        ss << " AND category='" << filter.get("category").to_str() << "'";
    }

    auto results = dbManager->query(ss.str());
    picojson::array arr;
    for (auto& row : results) {
        picojson::object obj;
        for (auto& col : row) obj[col.first] = picojson::value(col.second);
        arr.push_back(picojson::value(obj));
    }

    picojson::object resultJson;
    resultJson["status"] = picojson::value("success");
    resultJson["expenses"] = picojson::value(arr);
    return picojson::value(resultJson).serialize();
}

std::string ExpenseManager::updateExpense(const std::string& username, const picojson::value& data) {
    auto rows = dbManager->query("SELECT id FROM users WHERE username = '" + username + "'");
    if (rows.empty()) return "{\"status\":\"error\",\"message\":\"User not found\"}";
    std::string userId = rows[0]["id"];

    std::string id = data.get("id").to_str();
    txnManager->beginTransaction();

    bool success = dbManager->update("expenses", "id='" + id + "' AND user_id='" + userId + "'", {
        {"amount", data.get("amount").to_str()},
        {"category", data.get("category").to_str()},
        {"note", data.get("note").to_str()},
        {"date", data.get("date").to_str()}
    });

    if (success) {
        txnManager->commitTransaction();
        return "{\"status\":\"success\"}";
    } else {
        txnManager->rollbackTransaction();
        return "{\"status\":\"error\",\"message\":\"Update failed\"}";
    }
}

std::string ExpenseManager::deleteExpense(const std::string& username, const picojson::value& data) {
    auto rows = dbManager->query("SELECT id FROM users WHERE username = '" + username + "'");
    if (rows.empty()) return "{\"status\":\"error\",\"message\":\"User not found\"}";
    std::string userId = rows[0]["id"];

    std::string id = data.get("id").to_str();
    txnManager->beginTransaction();

    bool success = dbManager->remove("expenses", "id='" + id + "' AND user_id='" + userId + "'");
    if (success) {
        txnManager->commitTransaction();
        return "{\"status\":\"success\"}";
    } else {
        txnManager->rollbackTransaction();
        return "{\"status\":\"error\",\"message\":\"Delete failed\"}";
    }
}
