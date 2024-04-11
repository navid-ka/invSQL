#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <string>

class DB {
  public:
    DB() {
        sqlite3_open(fileInv, &inventory);
        activateForeignKeys();
        createInventory();
        createEquipment();
    }
    ~DB() { sqlite3_close(inventory); }

    int activateForeignKeys() {
        exit = sqlite3_exec(inventory, "PRAGMA foreign_keys = ON;", NULL, NULL,
                            &errormsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Error activating foreing_keys on inventory: "
                      << errormsg << std::endl;
            sqlite3_free(errormsg);
            return 1;
        }
        return 0;
    }

    int createInventory() {
        std::string sql = "CREATE TABLE IF NOT EXISTS INVENTORY("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "NAME      TEXT NOT NULL, "
                          "STATS     INT NOT NULL, "
                          "QTY       INT  NOT NULL, "
                          "GRADE     CHAR(1) );";

        try {
            exit = sqlite3_exec(inventory, sql.c_str(), NULL, 0, &errormsg);
            if (exit != SQLITE_OK) {
                std::cerr << "Error in createTable function." << std::endl;
                sqlite3_free(errormsg);
            } else
                std::cout << "Table Inventory created Successfully"
                          << std::endl;
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

    int selectFROM(std::string table, sqlite3 *db) {
        if (!db) {
            std::cerr << "Invalid database pointer." << std::endl;
        }

        std::ostringstream oss;
        oss << "SELECT * FROM " << table << ";";
        std::string sql = oss.str();
        exit = sqlite3_exec(db, sql.c_str(), callback, NULL, &errormsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in select function." << std::endl;
            sqlite3_free(errormsg);
        } else
            std::cout << "Selected Successfully FROM " << table << std::endl;
        return exit;
    }

    static int existsCallback(void *count, int argc, char **argv,
                              char **azColName) {
        int *flag = (int *)count;
        *flag = 1;
        return 1;
    }

    bool itemExists(std::string name) {
        int callback_count = 0;
        std::ostringstream oss;
        oss << "SELECT * FROM INVENTORY WHERE NAME ='" << name << "';";
        std::string sql = oss.str();
        exit = sqlite3_exec(inventory, sql.c_str(), existsCallback,
                            &callback_count, &errormsg);
        if (callback_count > 0)
            return true;
        else
            return false;
    }

    int updateQTYinv(std::string name, int qty) {

        std::ostringstream oss;
        oss << "UPDATE INVENTORY SET QTY = QTY + " << qty << " WHERE NAME ='"
            << name << "' AND QTY < 100;";
        std::string sql = oss.str();
        exit = sqlite3_exec(inventory, sql.c_str(), NULL, NULL, &errormsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Failed to update or reached max capacity"
                      << std::endl;
            return 1;
        } else {
            return 0;
        }
    }

    int addInventory(std::string name, int stats, int qty, std::string grade) {

        if (itemExists(name)) {
            updateQTYinv(name, qty);
        } else {
            std::ostringstream oss;
            oss << "INSERT INTO INVENTORY(NAME, STATS, QTY, GRADE) "
                   "VALUES('"
                << name << "', '" << stats << "', '" << qty << "', '" << grade
                << "');";
            std::string sql = oss.str();

            exit = sqlite3_exec(inventory, sql.c_str(), NULL, NULL, &errormsg);
            if (exit != SQLITE_OK) {
                std::cerr << "Error in addInventory function." << std::endl;
                sqlite3_free(errormsg);
            } else
                std::cout << "Inserted Successfully" << std::endl;
        }
        return exit;
    }

    int deleteItemFromInventory(std::string name) {
        // Let's treat the db as stack and delete the last ID added with name.
        std::ostringstream oss;
        oss << "DELETE FROM INVENTORY WHERE ID = (SELECT MAX(ID) FROM "
               "INVENTORY WHERE NAME = '"
            << name << "');";
        std::string sql = oss.str();

        exit = sqlite3_exec(inventory, sql.c_str(), callback, NULL, &errormsg);
        if (exit != SQLITE_OK)
            std::cerr << "Error deleting the item " << name << std::endl;
        else
            std::cout << "Item named: " << name << " deleted Successfully"
                      << std::endl;

        return exit;
    }
    // TODO::
    // int deleteEquipment(); <- this will bring object back to inventory
    // int selectEquipment(); equipment will have 8 slots

    int createEquipment() {
        std::string sql =
            "CREATE TABLE IF NOT EXISTS EQUIPMENT ("
            "ID         INTEGER PRIMARY KEY, "
            "ITEMNAME   TEXT NOT NULL, "
            "FOREIGN KEY (ITEMNAME) REFERENCES INVENTORY(NAME) );";
        exit = sqlite3_exec(inventory, sql.c_str(), NULL, NULL, &errormsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Error in createEquipment function." << std::endl;
            sqlite3_free(errormsg);
        } else
            std::cout << "Table equipment created Successfully" << std::endl;

        return 0;
    }

    sqlite3 *getDB() { return inventory; }

  private:
    sqlite3 *inventory;
    const char *fileInv = "inventory.db";
    char *errormsg;
    int exit;
};

int main() {
    DB db;

    // Inventory item
    std::string rustySword = "Rusty Sword";
    int swordStats = 4;
    int qty = 1;
    std::string gradeTier = "F";

    db.addInventory(rustySword, swordStats, qty, gradeTier);
    // inventory.deleteItemFromInventory(rustySword);
    db.selectFROM("INVENTORY", db.getDB());
    db.selectFROM("EQUIPMENT", db.getDB());

    return 0;
}
