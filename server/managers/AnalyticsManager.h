#ifndef ANALYTICSMANAGER_H
#define ANALYTICSMANAGER_H

#include "DBManager.h"
#include "TransactionManager.h"
#include "picojson.h"

class AnalyticsManager {
private:
    DBManager* dbManager;
    TransactionManager* txManager;

public:
    // ✅ FIXED: Only declare the constructor here — don't define it inline
    AnalyticsManager(DBManager* db, TransactionManager* tx);

    picojson::value getAnalytics(const std::string& userId);
};

#endif // ANALYTICSMANAGER_H
