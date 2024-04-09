#include <iostream>
#include <sqlite3.h>

class DB {
  public:
    DB() { exit = sqlite3_open(file, &db); }
    ~DB() { sqlite3_close(this->db); }

    int createTable() {
        std::string sql = "CREATE TABLE IF NOT EXISTS INVENTORY("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "NAME      TEXT NOT NULL, "
                          "STATS     INT NOT NULL, "
                          "QTY       INT  NOT NULL, "
                          "GRADE     CHAR(1) );";

        try {
            exit = sqlite3_exec(this->db, sql.c_str(), NULL, 0, &errormsg);
            if (exit != SQLITE_OK) {
                std::cerr << "Error in createTable function." << std::endl;
                sqlite3_free(errormsg);
            } else
                std::cout << "Table created Successfully" << std::endl;
        } catch (std::exception &e) {
            std::cerr << e.what();
        }
        return exit;
    }
    static int callback(void *NotUsed, int argc, char **argv,
                        char **azColName) {
        for (int i = 0; i < argc; i++) {
            std::cout << azColName[i] << ": " << argv[i] << std::endl;
        }
        std::cout << std::endl;
        return 0;
    }
    int selectInventory() {
        std::string sql = "SELECT * FROM INVENTORY;";

        exit = sqlite3_exec(this->db, sql.c_str(), callback, NULL, &errormsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in selectInventory function." << std::endl;
            sqlite3_free(errormsg);
        } else
            std::cout << "Selected Successfully" << std::endl;
        return exit;
    }

    int addInventory() {
        // TODO: pass the arguments by string
        std::string sql = "INSERT INTO INVENTORY(NAME, STATS, QTY, GRADE) "
                          "VALUES('Rusty Sword', '4', '1', 'F');";

        exit = sqlite3_exec(this->db, sql.c_str(), callback, NULL, &errormsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in selectInventory function." << std::endl;
            sqlite3_free(errormsg);
        } else
            std::cout << "Inserted Successfully" << std::endl;

        return exit;
    }

  private:
    sqlite3 *db;
    const char *file = "inventory.db";
    char *errormsg;
    int exit;
};

int main() {
    DB inventory;
    inventory.createTable();
    inventory.selectInventory();
    inventory.addInventory();

    return 0;
}
