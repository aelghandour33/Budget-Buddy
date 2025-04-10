cmake_minimum_required(VERSION 3.10)
project(BudgetBuddyServer)

set(CMAKE_CXX_STANDARD 17)

# Find OpenSSL
set(OPENSSL_ROOT_DIR /usr/local/opt/openssl-1.1)
find_package(OpenSSL REQUIRED)

# Include headers
include_directories(
    external
    managers
    models
    /usr/local/mysql-connector-c++-8.0.33/include
    /usr/local/mysql-connector-cpp/include
    ${OPENSSL_ROOT_DIR}/include
)

# Link directories
link_directories(
    /usr/local/mysql-connector-c++-8.0.33/lib64         # ✅ Corrected path
    /usr/local/mysql-connector-cpp/lib64                # ✅ Secondary path
    ${OPENSSL_ROOT_DIR}/lib
)

# Source files
add_executable(BudgetBuddyServer
    main.cpp
    managers/DBManager.cpp
    managers/TransactionManager.cpp
    managers/AuthManager.cpp
    managers/UserSessionManager.cpp
    managers/ExpenseManager.cpp
    managers/AnalyticsManager.cpp
)

# Link libraries
target_link_libraries(BudgetBuddyServer
    mysqlcppconn8                                        # ✅ Will now be found
    pthread
    OpenSSL::SSL
    OpenSSL::Crypto
)
