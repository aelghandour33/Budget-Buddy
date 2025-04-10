#include "UserSessionManager.h"

std::string UserSessionManager::createSession(const std::string& token, const std::string& userId) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    tokenMap[token] = userId;
    return token;
}

std::string UserSessionManager::getUserIdFromToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(sessionMutex);
    if (tokenMap.find(token) != tokenMap.end())
        return tokenMap[token];
    return "";
}
