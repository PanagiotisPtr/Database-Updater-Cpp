#ifndef STOCK_H
#define STOCK_H

#include <iostream>
#include <string>
#include <algorithm>
#include <jsoncpp/json/value.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <time.h>

#include "Options.h"
#include "Connection.h"

/*
    The Stock class is the centerpiece of this project. Given a Json::Value it allows easy posting of the data on the mysql database.
    It also handles duplicate values at the databases and updates them while adding the new ones as needed.
*/

class Stock{
public:
    Stock(const Json::Value &j): data(j){}
    void set_opt(std::string ip, std::string user, std::string pass, std::string db, unsigned port, const char *socket, unsigned long flag);
    void post(std::string database);
private:
    Json::Value data;
    // Stock props is a dummy struct that holds a few values and is used to avoid redundant code
    struct StockProps{
        std::string get_vals_string(){
            return unix_date + "," + open + "," + high + "," + low + "," + close + "," + volume;
        }
        std::string date;
        std::string unix_date;
        std::string open;
        std::string high;
        std::string low;
        std::string close;
        std::string volume;
    };

    //constants used for the progrss bar
    static const std::string PBSTR;
    static const unsigned PBWIDTH;

    // Helpers
    std::string get_unix(std::string date); // returns date as a unix timestamp
    void print_progress(double percentage); // updates progress bar
    bool create_table(std::string db_name, std::string table_name);
    StockProps parse_props(Json::Value::const_iterator i);

    // Connection options
    Options conn_opt;
};

#endif
