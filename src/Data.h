#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <string>
#include <algorithm>
#include <jsoncpp/json/value.h>
#include <thread>
#include <chrono>
#include <ctime>
#include <time.h>
#include <vector>

#include "Options.h"
#include "Connection.h"

/*
    Base class for all specialized json parsers. This class contains core functionality that all specialized classes need.
    For examlpe the News and Stock classes are similar in the sense that they need a lot of the same functions to operate
    such as post, set_opt, create_table etc. The only thing that changes is the way the data is collected from the json.
    Thus the create_data_query function is abstract and each specialized class only has to change this one function.
*/

class Data{
public:
    Data(const Json::Value &j): data(j){}
    void set_opt(std::string ip, std::string user, std::string pass, std::string db, unsigned port, const char *socket, unsigned long flag);
    void post(std::string table_name);
protected:
    Json::Value data;
    std::vector<std::pair<std::string, std::string> > props; // ex. {"Date", {"int unsigned", "342134123"}}

    // Helpers
    std::string get_props_string(bool with_type);
    std::string get_unix(std::string date); // returns date as a unix timestamp
    bool create_table(std::string table_name);
    virtual std::string create_data_query() = 0;

    // Connection options
    Options conn_opt;
};

#endif
