#ifndef EXPENSE_MANAGER_H
#define EXPENSE_MANAGER_H

#include "DBManager.h"
#include "TransactionManager.h"
#include <string>
#include "picojson.h"

class ExpenseManager {
public:
    ExpenseManager(DBManager* db, TransactionManager* txn);

    std::string addExpense(const std::string& userId, const picojson::value& data);
    std::string getExpenses(const std::string& userId, const picojson::value& filter);
    std::string updateExpense(const std::string& userId, const picojson::value& data);
    std::string deleteExpense(const std::string& userId, const picojson::value& data);

private:
    DBManager* dbManager;
    TransactionManager* txnManager;
};

#endif
