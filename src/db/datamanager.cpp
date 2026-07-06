#include "db/datamanager.hpp"
#include <iostream>

using namespace std;

bool DataManager::abrirSQL() {
    int rc = sqlite3_open("../data/cine.db", &db);

    if (rc != SQLITE_OK) {
        std::cerr << "No se pudo abrir: " << sqlite3_errmsg(db) << std::endl;
    }
    return rc == SQLITE_OK;
}

void DataManager::cerrarSQL() {
    if(db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}