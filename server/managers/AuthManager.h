#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>
#include "DBManager.h"
#include "TransactionManager.h"
#include "UserSessionManager.h"  // ✅ Include it

class AuthManager {
public:
    AuthManager(DBManager* db, TransactionManager* tx, UserSessionManager* session);
    bool loginUser(const std::string& username, const std::string& password, std::string& token);

private:
    DBManager* dbManager;
    TransactionManager* txManager;
    UserSessionManager* sessionManager;  // ✅ Add this
};

#endif // AUTHMANAGER_H
