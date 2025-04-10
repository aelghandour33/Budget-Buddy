#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include "DBManager.h"

class TransactionManager {
public:
    TransactionManager(DBManager* db);  // pointer to match main.cpp usage
    void beginTransaction();
    void commitTransaction();
    void rollbackTransaction();

private:
    DBManager* dbManager;  // pointer type to match constructor
};

#endif
