#include "AuthManager.h"
#include "jwt.h"
#include <iostream>

AuthManager::AuthManager(DBManager* db, TransactionManager* tx, UserSessionManager* session)
    : dbManager(db), txManager(tx), sessionManager(session) {}

bool AuthManager::loginUser(const std::string& username, const std::string& password, std::string& token) {
    try {
        txManager->beginTransaction();

        std::string query = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "'";
        std::cout << "[AuthManager] Executing query: " << query << std::endl;

        auto results = dbManager->query(query);
        std::cout << "[AuthManager] Query returned " << results.size() << " results" << std::endl;

        if (results.empty()) {
            std::cout << "[AuthManager] No matching user found.\n";
            txManager->rollbackTransaction();
            return false;
        }

        // Log the returned row data
        for (const auto& [key, value] : results[0]) {
            std::cout << "[AuthManager] " << key << ": " << value << std::endl;
        }

        // Generate JWT
        jwt::jwt_object jwtObj(
            jwt::params::algorithm("HS256"),
            jwt::params::secret("my_secret_key")
        );

        jwtObj.add_claim("username", username);
        jwtObj.add_claim("role", results[0].at("role"));  // assuming 'role' column exists

        token = jwtObj.signature();

        // Store session
        sessionManager->createSession(token, username);  // ✅ now works

        txManager->commitTransaction();
        std::cout << "[AuthManager] Login success. Token generated.\n";
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[AuthManager] Login error: " << e.what() << std::endl;
        txManager->rollbackTransaction();
        return false;
    }
}
