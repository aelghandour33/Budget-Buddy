#include "AnalyticsManager.h"
#include "picojson.h"
#include <iostream>

AnalyticsManager::AnalyticsManager(DBManager* db, TransactionManager* tx)
    : dbManager(db), txManager(tx) {}

picojson::value AnalyticsManager::getAnalytics(const std::string& username) {
    if (!dbManager || !txManager) {
        return picojson::value("Database error");
    }

    try {
        txManager->beginTransaction();

        std::string userQuery = "SELECT id FROM users WHERE username = '" + username + "'";
        auto userResult = dbManager->query(userQuery);

        if (userResult.empty() || userResult[0].find("id") == userResult[0].end()) {
            txManager->rollbackTransaction();
            return picojson::value("User not found");
        }

        std::string userId = userResult[0]["id"];

        std::cout << "[AnalyticsManager] Resolved user_id = " << userId << " from username = " << username << std::endl;

        std::string queryStr = "SELECT category, SUM(amount) as total FROM expenses WHERE user_id = '" + userId + "' GROUP BY category";
        auto results = dbManager->query(queryStr);

        picojson::object resultJson;
        for (const auto& row : results) {
            if (row.find("category") != row.end() && row.find("total") != row.end()) {
                resultJson[row.at("category")] = picojson::value(row.at("total"));
            }
        }

        txManager->commitTransaction();
        return picojson::value(resultJson);
    } catch (const std::exception& e) {
        std::cerr << "[AnalyticsManager] Exception: " << e.what() << std::endl;
        txManager->rollbackTransaction();
        return picojson::value("Error retrieving analytics");
    }
}
