#include "TransactionManager.h"

TransactionManager::TransactionManager(DBManager* db) : dbManager(db) {}

void TransactionManager::beginTransaction() {
    dbManager->startTransaction();
}

void TransactionManager::commitTransaction() {
    dbManager->commit();
}

void TransactionManager::rollbackTransaction() {
    dbManager->rollback();
}
