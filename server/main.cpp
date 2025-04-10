#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <thread>
#include "httplib.h"
#include "picojson.h"
#include "DBManager.h"
#include "TransactionManager.h"
#include "ExpenseManager.h"
#include "AuthManager.h"
#include "AnalyticsManager.h"
#include "UserSessionManager.h"

DBManager dbManager;
TransactionManager txManager(&dbManager);
ExpenseManager expenseManager(&dbManager, &txManager);
UserSessionManager sessionManager;
AuthManager authManager(&dbManager, &txManager, &sessionManager);
AnalyticsManager analyticsManager(&dbManager, &txManager);

std::mutex coutMutex;

std::string extractToken(const httplib::Request& req) {
    auto it = req.headers.find("Authorization");
    if (it != req.headers.end()) {
        const std::string& authHeader = it->second;
        if (authHeader.rfind("Bearer ", 0) == 0) {
            return authHeader.substr(7);
        }
    }
    return "";
}

int main() {
    httplib::Server svr;

    // ✅ Set CORS headers globally for all responses
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    // ✅ Handle CORS preflight requests (OPTIONS) — do NOT set duplicate origin
    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\033[1;36m[Thread " << std::this_thread::get_id()
                      << "]\033[0m 📋 Handling OPTIONS request" << std::endl;
        }

        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200;
    });

    // 🔐 Login
    svr.Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\033[1;36m[Thread " << std::this_thread::get_id()
                      << "]\033[0m 🔐 Login called" << std::endl;
        }

        picojson::value body;
        picojson::parse(body, req.body);

        std::string username = body.get("username").to_str();
        std::string password = body.get("password").to_str();
        std::string token;

        bool success = authManager.loginUser(username, password, token);
        picojson::object result;

        if (success) {
            sessionManager.createSession(token, username);
            result["status"] = picojson::value("success");
            result["token"] = picojson::value(token);
        } else {
            result["status"] = picojson::value("error");
            result["message"] = picojson::value("Invalid credentials");
        }

        res.set_content(picojson::value(result).serialize(), "application/json");
    });

    // 💸 Add expense
    svr.Post("/expenses", [](const httplib::Request& req, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\033[1;36m[Thread " << std::this_thread::get_id()
                      << "]\033[0m ➕ Add Expense called" << std::endl;
        }

        std::string token = extractToken(req);
        std::string username = sessionManager.getUserIdFromToken(token);

        picojson::value body;
        picojson::parse(body, req.body);
        std::string result = expenseManager.addExpense(username, body);
        res.set_content(result, "application/json");
    });

    // 📃 Get expenses
    svr.Get("/expenses", [](const httplib::Request& req, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\033[1;36m[Thread " << std::this_thread::get_id()
                      << "]\033[0m 📃 Get Expenses called" << std::endl;
        }

        std::string token = extractToken(req);
        std::string username = sessionManager.getUserIdFromToken(token);

        picojson::object dummyFilter;
        std::string result = expenseManager.getExpenses(username, picojson::value(dummyFilter));
        res.set_content(result, "application/json");
    });

    // ✏️ Update expense
    svr.Put("/expenses", [](const httplib::Request& req, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\033[1;36m[Thread " << std::this_thread::get_id()
                      << "]\033[0m ✏️ Update Expense called" << std::endl;
        }

        std::string token = extractToken(req);
        std::string username = sessionManager.getUserIdFromToken(token);

        picojson::value body;
        picojson::parse(body, req.body);
        std::string result = expenseManager.updateExpense(username, body);
        res.set_content(result, "application/json");
    });

    // ❌ Delete expense
    svr.Delete("/expenses", [](const httplib::Request& req, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\033[1;36m[Thread " << std::this_thread::get_id()
                      << "]\033[0m ❌ Delete Expense called" << std::endl;
        }

        std::string token = extractToken(req);
        std::string username = sessionManager.getUserIdFromToken(token);

        picojson::value body;
        picojson::parse(body, req.body);
        std::string result = expenseManager.deleteExpense(username, body);
        res.set_content(result, "application/json");
    });

    // 📊 Analytics
    svr.Get("/analytics", [](const httplib::Request& req, httplib::Response& res) {
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << "\033[1;36m[Thread " << std::this_thread::get_id()
                      << "]\033[0m 📊 Analytics requested" << std::endl;
        }

        std::string token = extractToken(req);
        std::string username = sessionManager.getUserIdFromToken(token);

        picojson::value result = analyticsManager.getAnalytics(username);
        res.set_content(result.serialize(), "application/json");
    });

    std::cout << "🚀 Server is running on http://localhost:5004" << std::endl;
    svr.listen("0.0.0.0", 5004);
}
