#ifndef STOCK_H
#define STOCK_H

#include "Data.h"

/*
    Specialized Stock class. All it does is parse the stock from the json that gets to collect the informtion necessary.
    Note: This classes is designed to only be used with the Alpha vantage API
    
    Even the comments are similar between the Stock and News classes... I wish I could use a inheritance to comment code too...
*/

class Stock : public Data {
public:
    Stock(const Json::Value &j): Data(j){
        props = {
            {"date", "int unsigned primary key"},
            {"open", "float"},
            {"high", "float"},
            {"low", "float"},
            {"close", "float"},
            {"volume", "int unsigned"}
        };
    }
private:
    std::string create_data_query(){
        std::string rv = "";
        for(Json::Value::const_iterator i = data["Time Series (Daily)"].end() ; i-- != data["Time Series (Daily)"].begin();){
            rv += "(";
            std::string date = i.key().asString();
            std::string unix_date = get_unix(date);
            std::string open = (*i).get("1. open", "missing_argument").asString();
            std::string high = (*i).get("2. high", "missing_argument").asString();
            std::string low = (*i).get("3. low", "missing_argument").asString();
            std::string close = (*i).get("4. close", "missing_argument").asString();
            std::string volume = (*i).get("5. volume", "missing_argument").asString();
            rv += unix_date + ", " + open + ", " + high + ", " + low + ", " + close + ", " + volume;
            rv += "),";
        }
        return rv;
    }
};

#endif
