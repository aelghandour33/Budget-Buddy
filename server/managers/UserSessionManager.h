#ifndef USERSESSIONMANAGER_H
#define USERSESSIONMANAGER_H

#include <string>
#include <unordered_map>
#include <mutex>

class UserSessionManager {
public:
    std::string createSession(const std::string& token, const std::string& userId);
    std::string getUserIdFromToken(const std::string& token);

private:
    std::unordered_map<std::string, std::string> tokenMap;
    std::mutex sessionMutex;
};

#endif
